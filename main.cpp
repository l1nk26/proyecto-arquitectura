#include <iostream>
#include <vector>
#include <map>
#include <random>
#include <memory>

#define BLOCK_SIZE 4096 // 4KB
#define MEGABYTE_SIZE 1024 * 1024
#define CACHE_SIZE 16 * MEGABYTE_SIZE // 16MB
#define NUM_ACCESSES 10000

// Clase base abstracta para sistemas de archivos
class FileSystem {
protected:
    int blockSize;
    std::vector<int> generatedAddresses;
    
public:
    FileSystem(int bs) : blockSize(bs) {}
    virtual ~FileSystem() = default;
    
    virtual void generateAccessPattern(int numAccesses) = 0;
    const std::vector<int>& getAddresses() const { return generatedAddresses; }
    int getBlockSize() const { return blockSize; }
};

// Implementación para ext3 (más accesos secuenciales)
class Ext3 : public FileSystem {
private:
    int currentBlock = 0;
    
public:
    Ext3(int bs) : FileSystem(bs) {}
    
    void generateAccessPattern(int numAccesses) override {
        generatedAddresses.clear();
        std::mt19937 gen(42); // Semilla fija para reproducibilidad
        std::bernoulli_distribution d(0.2); // 20% de aleatoriedad
        
        for(int i = 0; i < numAccesses; ++i) {
            if(d(gen)) {
                currentBlock += (rand() % 10 + 1); // Salto aleatorio
            } else {
                currentBlock++; // Secuencial
            }
            generatedAddresses.push_back(currentBlock * blockSize);
        }
    }
};

// Implementación para ext4 (más accesos aleatorios con delayed allocation)
class Ext4 : public FileSystem {
public:
    Ext4(int bs) : FileSystem(bs) {}
    
    void generateAccessPattern(int numAccesses) override {
        generatedAddresses.clear();
        std::mt19937 gen(123); // Semilla diferente
        std::uniform_int_distribution<> dis(0, 100000);
        
        for(int i = 0; i < numAccesses; ++i) {
            generatedAddresses.push_back(dis(gen) * blockSize);
        }
    }
};

class CacheSimulator {
protected:
    int cacheSize;
    int blockSize;
    int associativity;
    int hits = 0;
    int misses = 0;
    
    struct CacheEntry {
        int tag;
        int lastUsed;
    };
    
    std::vector<std::vector<CacheEntry>> sets;
    int timeCounter = 0;

    virtual bool accessBlock(int address) = 0;
    
public:
    CacheSimulator(int size, int bs, int assoc) 
        : cacheSize(size), blockSize(bs), associativity(assoc) {}
    
    virtual ~CacheSimulator() = default;
    
    void simulate(const FileSystem& fs) {
        for(int addr : fs.getAddresses()) {
            if(accessBlock(addr)) hits++;
            else misses++;
        }
    }
    
    void printStats(const std::string& name) const {
        std::cout << name << " - Hits: " << hits 
                  << ", Misses: " << misses 
                  << ", Hit Rate: " << (hits * 100.0 / (hits + misses)) << "%\n";
    }
    
    void reset() {
        hits = misses = 0;
        timeCounter = 0;
        for(auto& set : sets) {
            for(auto& entry : set) {
                entry.tag = -1;
                entry.lastUsed = 0;
            }
        }
    }
};

// Caché de correspondencia directa
class DirectMappedCache : public CacheSimulator {
public:
    DirectMappedCache(int size, int bs) 
        : CacheSimulator(size, bs, 1) {
        int numBlocks = size / bs;
        sets.resize(numBlocks, std::vector<CacheEntry>(1, {-1, 0}));
    }

private:
    bool accessBlock(int address) override {
        int blockNumber = address / blockSize;
        int index = blockNumber % sets.size();
        int tag = blockNumber / sets.size();
        
        timeCounter++;
        
        if(sets[index][0].tag == tag) {
            sets[index][0].lastUsed = timeCounter;
            return true;
        }
        
        sets[index][0].tag = tag;
        sets[index][0].lastUsed = timeCounter;
        return false;
    }
};

// Caché asociativa por vías (LRU)
class SetAssociativeCache : public CacheSimulator {
public:
    SetAssociativeCache(int size, int bs, int assoc) 
        : CacheSimulator(size, bs, assoc) {
        int numSets = size / (bs * assoc);
        sets.resize(numSets, std::vector<CacheEntry>(assoc, {-1, 0}));
    }

private:
    bool accessBlock(int address) override {
        int blockNumber = address / blockSize;
        int setIndex = blockNumber % sets.size();
        int tag = blockNumber / sets.size();
        
        auto& set = sets[setIndex];
        timeCounter++;
        
        // Buscar en el conjunto
        for(auto& entry : set) {
            if(entry.tag == tag) {
                entry.lastUsed = timeCounter;
                return true;
            }
        }
        
        // Miss: Reemplazar LRU
        int lruIndex = 0;
        for(int i = 1; i < set.size(); ++i) {
            if(set[i].lastUsed < set[lruIndex].lastUsed) {
                lruIndex = i;
            }
        }
        
        set[lruIndex].tag = tag;
        set[lruIndex].lastUsed = timeCounter;
        return false;
    }
};

int main() {
    
    const int ways = 2;

    Ext3 ext3(BLOCK_SIZE);
    Ext4 ext4(BLOCK_SIZE);
    
    ext3.generateAccessPattern(NUM_ACCESSES);
    ext4.generateAccessPattern(NUM_ACCESSES);

    // Crear cachés
    DirectMappedCache dmCache(CACHE_SIZE, BLOCK_SIZE);
    SetAssociativeCache saCache(CACHE_SIZE, BLOCK_SIZE, ways); // 2-way
    
    // Simular todas las combinaciones
    std::cout << "=== ext3 ===" << std::endl;
    dmCache.simulate(ext3);
    dmCache.printStats("Direct Mapped");
    dmCache.reset();
    
    saCache.simulate(ext3);
    saCache.printStats("2-Way Set Associative");
    saCache.reset();
    
    std::cout << "\n=== ext4 ===" << std::endl;
    dmCache.simulate(ext4);
    dmCache.printStats("Direct Mapped");
    dmCache.reset();
    
    saCache.simulate(ext4);
    saCache.printStats("4-Way Set Associative");
    
    return 0;
}