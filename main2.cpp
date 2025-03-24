#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <memory>

// Clase padre de sistemas de archivos
class FileSystem {
protected:
    int blockSize;
public:
    FileSystem(int bs) : blockSize(bs) {}
    virtual ~FileSystem() = default;
    virtual int translateAddress(int logicalAddress) const = 0;
    int getBlockSize() const { return blockSize; }
};

// Implementación para ext3 (traducción simple)
class Ext3 : public FileSystem {
public:
    Ext3(int bs) : FileSystem(bs) {}
    int translateAddress(int logicalAddress) const override {
        return logicalAddress; // Traducción directa (sin fragmentación)
    }
};

// Implementación para ext4 (simula delayed allocation con offsets)
class Ext4 : public FileSystem {
private:
    int allocationOffset;
public:
    Ext4(int bs) : FileSystem(bs), allocationOffset(0) {}
    int translateAddress(int logicalAddress) const override {
        return logicalAddress + (allocationOffset % 1024); // Simula dispersión
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

    virtual bool accessBlock(int physicalAddress) = 0;
    
public:
    CacheSimulator(int size, int bs, int assoc) 
        : cacheSize(size), blockSize(bs), associativity(assoc) {}
    
    virtual ~CacheSimulator() = default;
    
    void simulate(const FileSystem& fs, const std::vector<int>& addresses) {
        for(int addr : addresses) {
            int physicalAddr = fs.translateAddress(addr);
            if(accessBlock(physicalAddr)) hits++;
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

// Implementaciones de caché (DirectMappedCache y SetAssociativeCache igual que antes)
// ... (Usar las mismas implementaciones del código anterior)

// Función para leer direcciones desde archivo
std::vector<int> readAddressesFromFile(const std::string& filename) {
    std::vector<int> addresses;
    std::ifstream file(filename);
    int addr;
    while(file >> std::hex >> addr) {
        addresses.push_back(addr);
    }
    return addresses;
}
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
    

int main() {
    // Leer secuencia de direcciones
    std::vector<int> addresses = readAddressesFromFile("direcciones.txt");
    
    // Crear sistemas de archivos
    Ext3 ext3(4096);  // Bloque de 4KB
    Ext4 ext4(4096);
    
    // Crear cachés
    DirectMappedCache dmCache(16 * 1024 * 1024, 4096); // 16MB
    SetAssociativeCache saCache(16 * 1024 * 1024, 4096, 4); // 4-way
    
    // Simular todas las combinaciones
    std::cout << "=== ext3 ===" << std::endl;
    dmCache.simulate(ext3, addresses);
    dmCache.printStats("Direct Mapped");
    dmCache.reset();
    
    saCache.simulate(ext3, addresses);
    saCache.printStats("4-Way Set Associative");
    saCache.reset();
    
    std::cout << "\n=== ext4 ===" << std::endl;
    dmCache.simulate(ext4, addresses);
    dmCache.printStats("Direct Mapped");
    dmCache.reset();
    
    saCache.simulate(ext4, addresses);
    saCache.printStats("4-Way Set Associative");
    
    return 0;
}