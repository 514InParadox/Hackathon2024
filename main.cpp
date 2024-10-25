#include <cstddef>
#include <cstdlib>
#include <string>
#include<iostream>
#include<vector>
#include<cstring>
#include<queue>
#include<algorithm>
#include<fstream>
#include <unordered_map>
#include "utils.h"
#include "huffman.h"

#include "../includes/utils.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;
using LL = long long;
template<typename T>
void upd_max(T &a,T b) { if (a < b) a = b; }
template<typename T>
void upd_min(T &a,T b) { if (a > b) a = b; }
const int N=1e5+5;
#define pb push_back
int Is_array[N];//0:not array,1:1-dimension array,2:2-dimension array
int Count_times[N];
long long Max_value[N],Min_value[N];
std::unordered_map<LL, int> set[1000];
int width[N];
bool sgn[N];
bool huff[N];
const long long MAX_VALUE=1e18,MIN_VALUE=-1e18;
int xiabiao[2];
vector<string> string_list,compress_string;
string Line;
int get_id(char ch){
    if(ch>='A'&&ch<='Z') return ch-'A'+1;
    if(ch>='a'&&ch<='z') return ch-'a'+27;
    if(ch>='0'&&ch<='9') return ch-'0'+53;
    if(ch=='_') return 63;
    return 64;
}
char get_char(int id){
    if(id>=1 && id<=26) return 'A'+id-1;
    if(id>=27 && id<=52) return 'a'+id-27;
    if(id>=53 && id<=62) return '0'+id-53;
    if(id==63) return '_';
    return '?';
}
struct Trie{
    struct node{
        int fail,to[129];
    }tr[N];
    int tot,in[N],last_position,match[N];
    void clear(){
        for(int i=0;i<=tot;i++){
            memset(tr[i].to,0,sizeof(tr[i].to));
            tr[i].fail=0;
            in[i]=0;
            match[i]=0;
        }
        tot=0;
    }
    bool insert(int start,int end){
        bool flag=0;
        int l=end-start+1;
        int u=0;
        for(int i=start;i<=end;i++){
            if(Line[i]>=53 && Line[i]<=62){
                if(Line[i-1]==63 && (i==end || Line[i+1]==63)){
                    // printf("fuck\n");
                    if(xiabiao[0]==-1) xiabiao[0]=Line[i]-53;
                    else xiabiao[1]=Line[i]-53;
                    continue;
                }
            }
            if(!tr[u].to[Line[i]]){
                tr[u].to[Line[i]]=++tot;
                flag=1;
            }
            u=tr[u].to[Line[i]];
            last_position=u;
        }
        return flag;
    }
}T;
long long compress_value;
struct Suffix_Automaton{
    int to[N*2][129],fa[N*2],l[N*2];
    int tot;
    int cnt[N*2];
    const int inf=2*N-1;
    pair<int,int> index[N*2];
 
    Suffix_Automaton(){
        fa[0]=-1;
    }
    void clear(){
        for(int i=0;i<=tot;i++){
            memset(to[i],0,sizeof(to[i]));
            fa[i]=0;
            l[i]=0;
            cnt[i]=0;
            index[i]={0,0};
        }
        fa[0]=-1;
        tot=0;
    }
    int add(int c,int last,pair<int,int> position){
        if(to[last][c] && l[to[last][c]]==l[last]+1){
            cnt[to[last][c]]++;
            return to[last][c];
        }
        int p=last,np=++tot,nq;
        bool flag=0;
        l[np]=l[p]+1;cnt[np]++;
        index[np]=position;
        while(p!=-1&&!to[p][c]){
            to[p][c]=np;
            p=fa[p];
        }
        if(p==-1){
            fa[np]=0;
            return np;
        }
        else{
            int q=to[p][c];
            if(l[q]==l[p]+1){
                fa[np]=q;
                return np;
            }
            else{
                if(p==last){
                    flag=1,np=inf,tot--;
                }
                nq=++tot;
                l[nq]=l[p]+1;
                index[nq]=index[q];
                for(int i=0;i<129;i++) to[nq][i]=to[q][i];
                fa[nq]=fa[q];fa[np]=fa[q]=nq;
                while(p!=-1&&to[p][c]==q){
                    to[p][c]=nq;
                    p=fa[p];
                }
            }
        }
        return flag?nq:np;
    }
 
    vector<int> vec[N];
    void topu(){
        int maxl=0;
        for(int i=1;i<=tot;i++){
            vec[l[i]].pb(i);
            maxl=max(maxl,l[i]);
        }
        for(int i=maxl;i>=1;i--){
            for(auto u:vec[i]){
                cnt[fa[u]]+=cnt[u];
            }
            vec[i].clear();
        }
    }
    bool solve(){
        int maxvalue=0,maxvalue_id=-1;
        for(int i=1;i<=tot;i++){
            if(cnt[i]==1 || l[i]==1) continue;
            int value=(l[i]-1)*(cnt[i]-1);
            if(value>maxvalue){
                maxvalue=value;
                maxvalue_id=i;
            }
        }
        if(maxvalue_id==-1) return 0;
        int j=index[maxvalue_id].first,k=index[maxvalue_id].second-l[maxvalue_id]+1;
        string tmp(string_list[j].begin()+k,string_list[j].begin()+k+l[maxvalue_id]);
        compress_string.push_back(tmp);
        compress_value+=maxvalue;
        return 1;
    }
}SAM;
int pos[N],border[N];
string kmp(string a,string b,int ch){
    string res;
    int la=a.size(),lb=b.size();
    int j=0;
    for(int i=1;i<lb;i++){
        while(j&&b[i]!=b[j]) j=border[j];
        if(b[i]==b[j]) j++;
        border[i+1]=j; 
    }
    j=0;
    for(int i=0;i<la;i++){
        res.push_back(a[i]);
        while(j&&b[j]!=a[i]) j=border[j];
        if(b[j]==a[i]) j++;
        if(j==lb){
            res.erase(res.end()-lb,res.end());
            res.push_back(ch);
            j=0;
        }
    }
    return res;
}
void Compress(int k){
    SAM.clear();
    // printf("当前的字符串列表：\n");
    for(int i=0;i<string_list.size();i++){
        string s=string_list[i];

        // for(auto x:s) printf("%c",get_char(x));
        // printf("\n");

        for(int j=0;j<s.size();j++){
            if(j==0) pos[j]=SAM.add(s[j],0,{i,j});
            else pos[j]=SAM.add(s[j],pos[j-1],{i,j});
        }
    }
    // printf("-------------\n");
    SAM.topu();


    if(!SAM.solve()) return;


    string tmp=compress_string.back();
    for(int i=0;i<string_list.size();i++){
        string_list[i]=kmp(string_list[i],tmp,63+k);
    }



    // printf("被压缩的字符串：\n");
    // for(auto x:compress_string){
    //     for(auto y:x) printf("%c",get_char(y));
    //     printf("\n");
    // }
    // printf("-------------\n");
}

JSON_my json[N];
void Read_data(int json_id){
    json[json_id].vec.clear();
    int len=Line.length();
    for(int i=0;i<len;i++){
        if(Line[i]=='\"'){
            ++i;
            int start=i;
            xiabiao[0]=-1;xiabiao[1]=-1;
            for(;i<len;i++){
                if(Line[i]=='\"') break;
                Line[i]=get_id(Line[i]);
            }
            if(T.insert(start,i-1)){
                string tmp;
                for(int j=start;j<=i-1;j++){
                    if(Line[j]>=53 && Line[j]<=62){
                        if(Line[j-1]==63 && (j==i-1 || Line[j+1]==63)) continue;
                    }
                    tmp.push_back(Line[j]);
                }
                string_list.push_back(tmp);
                if(xiabiao[1]!=-1) Is_array[string_list.size()-1]=2;
                else if(xiabiao[0]!=-1) Is_array[string_list.size()-1]=1;
                else Is_array[string_list.size()-1]=0;
                T.match[T.last_position]=string_list.size()-1;
                Max_value[string_list.size()-1]=MIN_VALUE;
                Min_value[string_list.size()-1]=MAX_VALUE;
            }
            int keyid=T.match[T.last_position];
            JSON_my::data tmp;
            tmp.key=keyid;
            // printf("%d %d %d\n",xiabiao[0],xiabiao[1],Is_array[keyid]);
            tmp.dimension[0]=xiabiao[0];
            tmp.dimension[1]=xiabiao[1];
            json[json_id].vec.push_back(tmp);
            if((xiabiao[0]==0 || xiabiao[0]==-1) && (xiabiao[1]==0 || xiabiao[1]==-1)) Count_times[keyid]++;
        }
        else if(Line[i]==':'){
            long long tmp_val=0;
            bool flag=1;
            while(!(Line[i]>='0' && Line[i]<='9')) {
                if( Line[i] == '-' ) flag = 0;
                i++;
            }
            for(;i<len;i++){
                if(Line[i]==',' || Line[i]=='}') break;
                else tmp_val=tmp_val*10+(Line[i]^48);
            }
            tmp_val*=(flag)?1:-1;
            json[json_id].vec.back().value=tmp_val;
            // printf("! %lld %lld %lld\n",Max_value[json[json_id].vec.back().key],Min_value[json[json_id].vec.back().key],tmp_val);
            int key_id = json[json_id].vec.back().key;
            upd_max(Max_value[key_id], tmp_val);
            upd_min(Min_value[key_id], tmp_val);
            ++set[key_id][tmp_val];
        }
    }
}
#define EPOCH 1000
void decompress(const char *in_fn, const char *out_fn)
{
	std::ios::sync_with_stdio(false), std::cin.tie(nullptr);
	Bitstream bs;
	bs.syncFrom(in_fn);
	std::string rep[128];
	for (int i = 1; i < 64; ++i)
		rep[i] += enc_to_ch(i);
	struct children
	{
		int l, r;
		children() = default;
		children(int l, int r) : l(l), r(r) { }
	};
	std::vector<children> key_ch;
	struct key_t
	{
		std::string s;
		int n;
		bool sgn;
		bool huff;
	};
	std::vector<key_t> key;
	std::vector<children> val_ch;
	std::vector<std::uint64_t> val;
	std::ofstream out_fs(out_fn);
	{
		int idx = 64;
		while (true)
		{
			int c = bs.extract(7);
			if (!c) break;
			do
			{
				rep[idx] += rep[c];
				c = bs.extract(7);
			}
			while (c);
			++idx;
		}
	}
	{
		std::function<int ()> DFS = [&]() {
			int u = key_ch.size();
			key.emplace_back();
			if (bs.extract(1))
			{
				key_ch.emplace_back();
				int l = DFS(), r = DFS();
				key_ch[u] = {l, r};
			}
			else
			{
				key_ch.emplace_back(0, 0);
				key_t &k = key.back();
				while (true)
				{
					int c = bs.extract(7);
					if (!c) break;
					k.s += rep[c];
				}
				k.n = bs.extract(6) + 1;
				k.sgn = bs.extract(1);
				k.huff = bs.extract(1);
			}
			return u;
		};
		if (bs.extract(1)) DFS();
	}
	{
		std::function<int ()> DFS = [&]() {
			int u = val_ch.size();
			if (bs.extract(1))
			{
				val_ch.emplace_back();
				val.emplace_back();
				int l = DFS(), r = DFS();
				val_ch[u] = {l, r};
			}
			else
			{
				val_ch.emplace_back(0, 0);
				val.push_back(bs.extract(8 << bs.extract(2)));
			}
			return u;
		};
		if (bs.extract(1)) DFS();
	}
	{
		auto infer_dim_cnt = [](const std::string &s) {
			int cnt = 0;
			for (std::size_t i = 0; i < s.size(); ++i)
				if (s[i] == '_' && (s[i + 1] == '_' || i + 1 == s.size()))
					++cnt;
			return cnt;
		};
		auto insert_dim_print = [&](const std::string &s, int *dim) {
			for (std::size_t i = 0; i < s.size(); ++i)
			{
				out_fs << s[i];
				if (s[i] == '_' && (s[i + 1] == '_' || i + 1 == s.size()))
					out_fs << *dim++;
			}
		};
		while (true)
		{
			bool first = true;
			while (true)
			{
				int u = 0;
				while (key_ch[u].l)
					if (!bs.extract(1))
						u = key_ch[u].l;
					else
						u = key_ch[u].r;
				auto &k = key[u];
				if (k.s.empty())
					if (first)
						return;
					else
						break;
				int dim_cnt = infer_dim_cnt(k.s);
				std::function<void (std::uint64_t)> print_value;
				if (k.sgn)
					print_value = [&](std::uint64_t x) {
						out_fs << static_cast<std::int64_t>(x >> k.n - 1 & 1 ? ~0ll << k.n | x : x);
					};
				else
					print_value = [&](std::uint64_t x) {
						out_fs << x;
					};
				std::function<std::uint64_t ()> parse_value;
				if (k.huff)
					parse_value = [&]() {
						int u = 0;
						while (val_ch[u].l)
							if (!bs.extract(1))
								u = val_ch[u].l;
							else
								u = val_ch[u].r;
						return val[u];
					};
				else
					parse_value = [&]() {
						return bs.extract(k.n);
					};
				auto dim = std::make_unique<int[]>(dim_cnt);
				std::function<void (int)> DFS = [&](int i) {
					if (i == dim_cnt)
					{
						if (first)
						{
							first = false;
							out_fs << "{ ";
						}
						else
							out_fs << ", ";
						out_fs << "\"", insert_dim_print(k.s, dim.get()), out_fs << "\"";
						out_fs << ": ";
						print_value(parse_value());
						return;
					}
					int ni = bs.extract(3);
					for (int j = 0; j <= ni; ++j)
						dim[i] = j, DFS(i + 1);
				};
				DFS(0);
			}
			out_fs << " }\n";
		}
	}
}
int main(int argc, const char **argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
	if (!(argc == 4 && (!strcmp(argv[1], "-compress") || !strcmp(argv[1], "-uncompress"))))
	{
		std::cerr << "Usage:\nmain -compress input_file output_path\nmain -uncompress input_file output_file\n";
		return 1;
	}
	if (argv[1][1] == 'u')
	{
		decompress(argv[2], argv[3]);
		return 0;
	}
	const char *in_fn = argv[2], *out_p = argv[3];
    clock_t start=clock();
    // ifstream file("smallData.txt");
    ifstream file(in_fn);
	std::system((std::string("mkdir -p ") + out_p).c_str());
	std::string in_fn_fn(in_fn);
	in_fn_fn = in_fn_fn.substr(in_fn_fn.rfind('/') + 1);

    int flush_file_count = 0;

    constexpr int bound[] = {8, 16, 32, 64};
    const int *bound_p = bound;
    int u_b[64];
    for (int i = 0; i < 64; ++i)
    {
        if (i == *bound_p)
            ++bound_p;
        u_b[i] = *bound_p;
    }
    while(1){
        int file_num=0;
        for(int i=0;i<string_list.size();i++){
            Is_array[i]=0;
            Count_times[i]=0;
        }
        string_list.clear();compress_string.clear();
        T.clear();
        while (file_num < EPOCH && getline(file, Line) && !Line.empty() && Line[0] == '{') {
            Read_data(file_num);
            file_num++;
            // for(auto x:json[file_num-1].vec){
            //     printf("%d %d %d %lld\n",x.key,x.dimension[0],x.dimension[1],x.value);
            // }
        }
        if (!file_num) break;
        
        
        // for(int i=0;i<string_list.size();i++){
        //     for(auto x:string_list[i]) printf("%c",get_char(x));
        //     printf(" %d %d\n",Is_array[i],Count_times[i]);
        // }
        for(int i=1;i<=43;i++){
            Compress(i);
        }
        auto bit_width = [](LL x) {
            int w = 0;
            while (1ll << w <= x >> 1)
                ++w;
            return w + 1;
        };
        for (int i = 0; i < string_list.size(); ++i)
        {
            if (Max_value[i] < 0)
                width[i] = bit_width(-(Min_value[i] + 1)) + 1;
            else if (Min_value[i] < 0)
                width[i] = std::max(bit_width(-(Min_value[i] + 1)), bit_width(Max_value[i])) + 1;
            else
                width[i] = bit_width(Max_value[i]);
            // printf("%d\n", Min_value[i]);
            sgn[i] = Min_value[i] < 0;
            int size = set[i].size();
            huff[i] = 2 * size - 1 + size * (2 + u_b[width[i]]) < static_cast<std::ptrdiff_t>(string_list.size()) * (width[i] - (bit_width(size - 1) + 4));
            // huff[i] = false;
        }
        keyHuff.reset();
        valueHuff.reset();
        // puts("A");
        compressOutputSpecialChar(compress_string);
        // puts("B");
        compressOutputKeyHuffman(string_list, Is_array, Count_times, width, sgn, huff, file_num);
        // puts("C");
        compressOutputValueHuffman(set, string_list.size(), huff);
        // puts("D");
        compressOutputJSON(json, file_num);
        // outFile.flushFake("dataFake.txt");
        // puts("E");
        // printf("-------------\n");
        // outFile.flushInto("dataTruth.txt");
        outFile.flushInto(std::string(out_p) + "/" + in_fn_fn + "_" + std::to_string(flush_file_count++));
        for(int i=0;i<string_list.size();i++){
            set[i].clear();
            // for(auto x:string_list[i]) printf("%c",get_char(x));
            // printf(" %d %d %lld %lld\n",Is_array[i],Count_times[i],Max_value[i],Min_value[i]);
        }
        // break;
        if(file_num!=EPOCH) break;
    }
    
    return 0;
}
