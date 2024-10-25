#include<iostream>
#include<vector>
#include<cstring>
#include<queue>
#include<algorithm>
using namespace std;
const int N=1e5+5;
#define pb push_back
vector<int> val;
vector<string> string_list,compress_string;
bool been_ignored[N];
int get_id(char ch){
    if(ch>='a'&&ch<='z') return ch-'a';
    if(ch>='A'&&ch<='Z') return ch-'A'+26;
    if(ch>='0'&&ch<='9') return ch-'0'+52;
    return ch-'z'+62;
}
struct Trie{
    struct node{
        int fail,to[100],ans;
    }tr[N];
    int tot,in[N];
    bool insert(string s){
        bool flag=0;
        int l=s.length();
        int u=0;
        for(int i=0;i<l;i++){
            if(!tr[u].to[get_id(s[i])]){
                tr[u].to[get_id(s[i])]=++tot;
                flag=1;
            }
            u=tr[u].to[get_id(s[i])];
        }
        return flag;
    }
}T;
struct Suffix_Automaton{
    int to[N*2][100],fa[N*2],l[N*2];//fa 和 lnk 是一个东西
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
                for(int i=0;i<100;i++) to[nq][i]=to[q][i];
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
    void solve(){
        int maxvalue=0,maxvalue_id;
        for(int i=1;i<=tot;i++){
            if(cnt[i]==1 || l[i]==1) continue;
            int value=(l[i]-1)*(cnt[i]-1);
            if(value>maxvalue){
                maxvalue=value;
                maxvalue_id=i;
            }
        }
        int j=index[maxvalue_id].first,k=index[maxvalue_id].second-l[maxvalue_id]+1;
        compress_string.push_back(string_list[j].substr(k,l[maxvalue_id]));
    }
}SAM;
int pos[N],border[N];
string kmp(string a,string b,char ch){//a 为原串（长），b 为需要被匹配的串（短）
    string res;
    int la=a.length(),lb=b.length();
    int j=0;
    for(int i=1;i<lb;i++){
        while(j&&b[i]!=b[j]) j=border[j];
        if(b[i]==b[j]) j++;
        border[i+1]=j; 
    }
    j=0;
    for(int i=0;i<la;i++){
        res+=a[i];
        while(j&&b[j]!=a[i]) j=border[j];
        if(b[j]==a[i]) j++;
        if(j==lb){
            res.replace(res.length()-lb,lb,1,ch);
            j=0;
        }
    }
    return res;
}
void Compress(int k){
    SAM.clear();
    printf("当前的字符串列表：\n");
    for(int i=0;i<string_list.size();i++){
        string s=string_list[i];
        printf("%s\n",s.c_str());
        for(int j=0;j<s.length();j++){
            if(j==0) pos[j]=SAM.add(get_id(s[j]),0,{i,j});
            else pos[j]=SAM.add(get_id(s[j]),pos[j-1],{i,j});
        }
    }
    printf("-------------\n");
    SAM.topu();
    SAM.solve();
    string tmp=compress_string.back();
    for(int i=0;i<string_list.size();i++){
        string s=string_list[i];
        string_list[i]=kmp(s,tmp,'z'+k);
    }
    printf("被压缩的字符串：\n");
    for(auto x:compress_string){
        printf("%s\n",x.c_str());
    }
    printf("-------------\n");
}
void get_string(){
    string s;
    char ch=getchar();
    while(ch!='\"'){
        if(ch>='0'&&ch<='9') ch=getchar();
        else{
            s+=ch;
            ch=getchar();
        }
    }
    if(T.insert(s)){
        string_list.push_back(s);
        // printf("%s\n",s.c_str());
    }
}
void Read_data(){
    char ch;
    do{
        int tmp;
        ch=getchar(); // 读 { 或 空格
        ch=getchar(); // 读 "
        get_string();
        scanf(": %d",&tmp);
        val.push_back(tmp);

        ch=getchar(); // 读 ,
        // cout<<ch<<endl; 
    }while(ch!='}');
    // printf("-------------\n");
}
int main(){
    for(int i=1;i<=5;i++){
        Read_data();
        getchar(); // 读换行
    }
    for(int i=1;i<=10;i++){
        Compress(i);
    }
    // for(auto x:compress_string){
    //     printf("%s\n",x.c_str());
    // }
    // printf("------------\n");
    printf("接受\n");
    return 0;
}
/*
logtype
Year
Month
Day
Hour
Minute
Second
MS
gNodeBID
CellID
CallId
PLMNID
PhysCellId
SerCarrierFreq
{SsbIndex
{RsrpRslt
{RsrqRslt
{SinrRslt
|PhysCellId
|{SsbIndex
|{RsrpRslt
|{RsrqRslt
|{SinrRslt
*/