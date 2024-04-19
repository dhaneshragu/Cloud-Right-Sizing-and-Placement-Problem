#include <bits/stdc++.h>
using namespace std;

int globalB,globalS;
struct node
{
    int B,S;
    int lastTime=0;
    multiset<int>VMs;
    set<int>chunks;
    node(int Bval,int Sval)
    {
        B= Bval;
        S= Sval;
        for(int i=0;i<S;i++)
        {
            VMs.insert(0);
        }
    }
};

struct deadline
{
    vector<pair<int,int>> chunks;
    int time=0;
    int totalChunks=0;
    int lastChunkTime=0;
    deadline(int t)
    {
        time = t;
    }
};

vector<node>deadlineOblivious(vector<deadline>&deadlines)
{
    vector<node>ans(1,node(globalB,globalS));
    for(int i=0;i<deadlines.size();i++)
    {
        cout<<"For Job "<<i+1<<endl<<endl;
        int jobEndTime=0;
        for(int j=0;j<deadlines[i].chunks.size();j++)
        {
            int startime=*ans[ans.size()-1].VMs.begin();
            if(ans[ans.size()-1].chunks.size()==globalB)
            startime=0;
            cout<<"Start Time for chunk "<<deadlines[i].chunks[j].first<<" is:- "<<startime<<endl;
            deadlines[i].totalChunks+=deadlines[i].chunks[j].second;
            if(ans[ans.size()-1].chunks.size()<globalB)
            {
                auto it=ans[ans.size()-1].VMs.begin();
                int endTime = *it+deadlines[i].chunks[j].second;
                ans[ans.size()-1].VMs.erase(it);
                ans[ans.size()-1].VMs.insert(endTime);
                cout<<"End Time for chunk "<<deadlines[i].chunks[j].first<<" is:- "<<endTime<<endl;
                jobEndTime=max(jobEndTime,endTime);
                ans[ans.size()-1].chunks.insert(deadlines[i].chunks[j].first);
            }
            else
            {
                for(auto x:ans[ans.size()-1].VMs)
                {
                    ans[ans.size()-1].lastTime = max(ans[ans.size()-1].lastTime,x);
                }
                node machine(globalB,globalS);
                machine.chunks.insert(deadlines[i].chunks[j].first);
                ans.push_back(machine);
                auto it=ans[ans.size()-1].VMs.begin();
                int endTime = *it+deadlines[i].chunks[j].second;
                ans[ans.size()-1].VMs.erase(it);
                ans[ans.size()-1].VMs.insert(endTime);
                cout<<"End Time for chunk "<<deadlines[i].chunks[j].first<<" is:- "<<endTime<<endl;
                jobEndTime=max(jobEndTime,endTime);
                ans[ans.size()-1].chunks.insert(deadlines[i].chunks[j].first);
            }
            deadlines[i].lastChunkTime = max(deadlines[i].lastChunkTime, jobEndTime);
        }
        cout<<endl;
    }
    return ans;
}
int main()
{
    int k;
    cin>>globalB;
    cin>>globalS;
    cin>>k;
    int jobID,dline,num;
    vector<deadline>deadlines;
    for(int i=0;i<k;i++)
    {
        cin>>jobID>>dline>>num;
        deadline d(dline);
        map<int,int>countOfChunks;
        int x;
        for(int j=0;j<num;j++)
        {
            cin>>x;
            countOfChunks[x]++;
        }
        for(auto chunk:countOfChunks)
        {
            d.chunks.push_back({chunk.first,chunk.second});
        }
        deadlines.push_back(d);
    }
    cout<<endl;
    vector<node>ans = deadlineOblivious(deadlines);  
    int deadlinesMissed=0;
    for(int i=0;i<deadlines.size();i++)
    {
        cout<<"Job "<<i+1<<" completed at time :- "<<deadlines[i].lastChunkTime<<endl;
        if(deadlines[i].lastChunkTime >= deadlines[i].time)
        deadlinesMissed++;
    }
    cout<<endl;
    cout<<"Total Number of Jobs whose deadlines missed:- "<<deadlinesMissed<<endl;
    cout<<"Total number of machines used:- "<<ans.size()<<endl;
}
