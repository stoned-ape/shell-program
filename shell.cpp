#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/wait.h>
#include <csignal>
#include <vector>
using namespace std;

int id=-1;
bool esc=false;
vector<string> hist(1,"");

string arrow(string s){
    int sz=hist.size();
    int ssz=s.size();
    int it=sz;
    string a=s;
    for(int i=0;i<ssz-2;i++){
        if(s[i]==27 && s[i+1]==91){
            if(s[i+2]==65){ //up
                it=max(it-1,0);
                a=hist[it];
            }else if(s[i+2]==66){ //down
                it=min(it+1,sz-1);
                a=hist[it];
            }
        }
    }
    return a;
}

char **parseargs(string s){
    int j=0,k=0;
    char **a=new char*[256];
    for(int i=0;i<s.size();i++)
        if(s[i]==' ' && s[i+1]==' ') s.erase(s.begin()+i--);
    for(int i=0;i<s.size();i++){
        if((s[i]==' ' || s[i]=='\n') && i>j){
            string ss=s.substr(j,i-j);
            a[k]=new char[ss.size()+1];
            for(int l=0;l<ss.size();l++) a[k][l]=ss[l];
            a[k][ss.size()]=0;
            j=i+1;
            k++;
        }
    }
    for(int i=k;i<256;i++) a[i]=NULL;
    return a;
}

void onsignal(int signum){
    if(id<=0) exit(0);
    esc=true;
    kill(id,SIGINT);
}

int main(int argc, char **argv, char **envpp){
    signal(SIGINT,onsignal);
    cout<<"enter \"quit\" to quit.\n";
tag:
    while(1){
        cout<<getenv("USER")<<" > ";
        string s="";
        char c;
        do{
            c=cin.get();
            s+=c;
            if(esc){
                esc=false;
                goto tag;
            }
        }while(c!='\n');
        if(s=="\n") continue;
        s=arrow(s);
        if(s=="hist\n"){
            for(auto i:hist) cout<<i;
            continue;
        }
        if(s.substr(0,4)=="quit") exit(0);
        hist.push_back(s);
        if(s.substr(0,2)=="cd"){
            char **args=parseargs(s);
            if(args[1]==NULL) chdir(getenv("HOME"));
            else chdir(args[1]);
            continue;
        }
        if((id=fork())==0){
            char **args=parseargs(s);
            if(execvp(args[0],args)<0) cout<<"invalid command"<<endl;
            exit(0);
        }
        wait(nullptr);
    }
}



