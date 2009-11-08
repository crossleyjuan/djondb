#include <vector>
#include <map>
#include <string>
#include <ctime>

using namespace std;

namespace cache {

    class CacheItem {
    private:
        long lastTimeUsed;
        string key;
        void* value;

    public:
        void updateLastTimeUsed();
        long getLastTimeUsed();
        CacheItem();
        CacheItem(string key, void* value);
        string getKey();
        void setKey(string key);
        void* getValue();
        void setValue(void* value);
    };

    class CacheGroup {
    private:
        map<string, CacheItem*> items;

    public:
        CacheGroup();
        
        void add(string key, void* value);

        void* get(string key);

        void remove(string key);
        
        int size();
    };

    class Cache {
    private:
        map<string, CacheGroup*> groups;

    public:
        CacheGroup get(string key);
    };

    CacheGroup getGlobalCache(string group);

    CacheGroup* getRuntimeCache();
}
