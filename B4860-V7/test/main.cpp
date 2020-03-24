#include <iostream>

#include <map>
#include <vector>
#include <set>
#include <cstring>

#include <regex>

using namespace std;

#if 0
typedef std::map<std::string, std::string> STR2STR_MAP;
typedef std::pair<std::string, std::string> STR2STR_PAIR;

void SplitStrings(const std::string &input, STR2STR_MAP &map, char sep_major = '&', char sep_minor = '=')
{
    char *euq, *amp, *start, *tmp;
    char *pairs = new char[input.size() + 4];
    memcpy(pairs, input.c_str(), input.size());
    memcpy(pairs + input.size(), &sep_major, 1);
    pairs[input.size()+1] = '/0';
    start = pairs;   
    for(amp = strchr(start, sep_major); amp != NULL; amp = strchr(start, sep_major))
    {
        *amp = '/0';
        euq = strchr(start, sep_minor);
        if(euq == NULL) break;
        *euq = '/0';
        euq++;

        tmp = amp-1;
        while((tmp != start) && ((*tmp == ' ') || (*tmp == '/t'))) tmp--; // escape ending space
        while((*start == ' ') || (*start == '/t')) start++; // escape leading space
        if((*start != '/0') && (*euq != '/0'))
        {
            std::string key(start), value(euq);
            map.insert(STR2STR_PAIR(key, value));
        }
        start = ++amp;
    }
    delete []pairs;
}
#endif

#if 1
/* c++11 字符串切割 */
vector<string> testSplit11(const string& in, const string& delim)
{
    vector<string> ret;
    try
    {
        regex re{delim};
        return vector<string>{
            sregex_token_iterator(in.begin(), in.end(), re, -1),
            sregex_token_iterator()
        };      
    }
    catch(const std::exception& e)
    {
        cout<<"error:"<<e.what()<<std::endl;
    }
    return ret;
}
#endif

#if 1
/* 实现key-value数据插入及修改 */
typedef struct atom_node
{
    std::string key;
    std::string value;

} atom;
typedef std::pair <std::string, struct atom_node> _KV_ ;

#if 0
//typedef std::map <std::string, std::string> KVM;
void SplitStrings2Map(const std::string &input, std::string sep_major = "&", std::string sep_minor = "=")
{
    int base = 2;
    std::map<std::string, atom> rhub_delay_up;
    std::map<std::string, atom> rhub_delay_down;
    std::map<std::string, atom> t14_delay;

    std::string key;
    atom v;
    int cout = 0;
    vector<string>ret = testSplit11(input.c_str(), sep_major);
    for(int i = 0; i < ret.size(); i++)
    {
        /* key = type + delay num  
         * type: rhub_delay_up(1) rhub_delay_down(2) t14_delay(3) 
         * delay num: 1-9
         */
        cout = (i % base) + 1;
        if(i < base)
        {
            key = std::string("1" + to_string(cout));
            vector<string>kv = testSplit11(ret[i].c_str(), sep_minor);
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            rhub_delay_up.insert(_KV_(key, v));
        } else if((i >= base) && (i < (base*2)))
        {
            key = std::string("2" + to_string(cout));
            vector<string>kv = testSplit11(ret[i].c_str(), sep_minor);
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            rhub_delay_down.insert(_KV_(key, v));
        } else if((i >= (base*2)) && (i < (base*3)))
        {
            key = std::string("3" + to_string(cout));
            vector<string>kv = testSplit11(ret[i].c_str(), sep_minor);
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            t14_delay.insert(_KV_(key, v));
        }
    }
    
    for(auto &it : rhub_delay_up)
    {
        std::cout << "rhub_delay_up.key=" << it.first 
                  << " rhub_delay_up.key.key=" << it.second.key
                  << " rhub_delay_up.key.value=" << it.second.value << std::endl;
    }

    for(auto &it : rhub_delay_down)
    {
        std::cout << "rhub_delay_down.key=" << it.first 
                  << " rhub_delay_down.key.key=" << it.second.key
                  << " rhub_delay_down.key.value=" << it.second.value << std::endl;
    }

    for(auto &it : t14_delay)
    {
        std::cout << "t14_delay.key=" << it.first 
                  << " t14_delay.key.key=" << it.second.key
                  << " t14_delay.key.value=" << it.second.value << std::endl;
    }

}
#endif

#if 1
void SplitStrings2Map_2(const std::string &input, std::string rruid, std::map<std::string, atom>& map, std::string sep_major = "&", std::string sep_minor = "=")
{
    int base = 2;

    std::string key;
    atom v;
    int cout = 0;
    vector<string>ret = testSplit11(input.c_str(), sep_major);
    int ret_size = ret.size();
    for(int i = 0; i < ret_size; i++)
    {
        /* key = type + delay num  
         * type: rhub_delay_up(1) rhub_delay_down(2) t14_delay(3) 
         * delay num: 1-9
         */
        cout = (i % base) + 1;
        if(i < base)
        {
            key = std::string(rruid + "1" + to_string(cout));
            vector<string>kv = testSplit11(ret[i].c_str(), sep_minor);
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            map.insert(_KV_(key, v));
        } else if((i >= base) && (i < (base*2)))
        {
            key = std::string(rruid + "2" + to_string(cout));
            vector<string>kv = testSplit11(ret[i].c_str(), sep_minor);
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            map.insert(_KV_(key, v));
        } else if((i >= (base*2)) && (i < (base*3)))
        {
            key = std::string(rruid + "3" + to_string(cout));
            vector<string>kv = testSplit11(ret[i].c_str(), sep_minor);
            v.key = kv[0].c_str();
            v.value = kv[1].c_str();
            map.insert(_KV_(key, v));
        }
    }
}
#endif

#if 1
void delete_hub_delay(std::string rruid, std::map<std::string, atom>& map)
{
    int base = 2;
    std::string key;
    int cout = 0;
    for(int i = 0; i < (base*3); i++)
    {
        cout = (i % base) + 1;
        key = std::string(rruid + to_string((i/base)+1) + to_string(cout));
        //std::cout << "key=" << key << std::endl;
        map.erase(key);
    }
}
#endif

#endif


int main()
{
    std::string data = "delay1_up=24&delay2_up=25&delay1_down=26&delay2_down=27&t14_delay1=11456&t14_delay2=11457";
#if 0
    STR2STR_MAP map;
    SplitStrings(data, map);
    for(auto &it : map)
    {
        std::cout << "key=" << it.first << " value=" << it.second << std::endl;
    }
#endif

#if 0
    vector<string>ret = testSplit11(data.c_str(), "&");
    for(int i = 0; i < ret.size(); i++)
    {
        std::cout << ret[i] << std::endl;
    }
#endif

    //SplitStrings2Map(data);
    
    std::string rruid = "1";
    std::map<std::string, atom> delay_map;
    SplitStrings2Map_2(data, rruid, delay_map);

    rruid = "2";
    SplitStrings2Map_2(data, rruid, delay_map);

    for(auto &it : delay_map)
    {
        std::cout << "key=" << it.first 
                  << " key.key=" << it.second.key
                  << " key.value=" << it.second.value << std::endl;
    }


    rruid = "1";
    delete_hub_delay(rruid, delay_map);
    std::cout << "/t----------------------" << std::endl;
    for(auto &it : delay_map)
    {
        std::cout << "key=" << it.first 
                  << " key.key=" << it.second.key
                  << " key.value=" << it.second.value << std::endl;
    }

    return 0;
}

