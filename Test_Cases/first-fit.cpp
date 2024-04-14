#include <bits/stdc++.h>
using namespace std;

int globalB,globalS;
struct Compare {
    bool operator()(const pair<int, int>& a, const pair<int, int>& b) const {
        if (a.first != b.first) {
            return a.first > b.first; // Sort by first element in descending order
        } else {
            return a.second > b.second; // If first elements are equal, sort by second element in descending order
        }
    }
};

struct node
{
    int B,S;
    multiset<pair<int,int>,Compare>deadline;
    set<int> chunks;
    node(int Bval,int Sval)
    {
        B= Bval;
        S= Sval;
    }
};

struct deadline
{
    vector<pair<int,int>> chunks;
    int timeleft;
    deadline(int t)
    {
        timeleft = t;
    }
};

vector<node>firstFit(vector<deadline>&deadlines)
{
    vector<node>ans(1,node(globalB,globalS));
    // cout<<ans[0].B<<" "<<ans[0].S<<"    ";
    // map<int,int>jobDeadlines;
    for(int i=0;i<deadlines.size();i++)
    {
        int ogDeadline = deadlines[i].timeleft;
        // for(int j=0;j<deadlines[i].chunks.size();j++)
        // {
        //     cout<<deadlines[i].chunks[j].first<<" "<<deadlines[i].chunks[j].second<<"   ";
        // }
        // cout<<endl;
        cout<<"Job "<<i+1<<" Statistics:-\n";
        for(int j=0;j<deadlines[i].chunks.size();j++)
        {
            auto start=ans[ans.size()-1].deadline.begin();
            int startime;
            if(ans[ans.size()-1].deadline.size()<globalS)
            {
                ans[ans.size()-1].deadline.insert({ogDeadline,i+1});
                startime=0;
                cout<<"Start time for chunk "<<deadlines[i].chunks[j].second<<" is:- 0\n";
            }
            else
            {
                startime=deadlines[start->second-1].timeleft-(start->first);
                cout<<"Start time for chunk "<<deadlines[i].chunks[j].second<<" is:- "<<startime<<"\n";
            }
            if(deadlines[i].chunks[j].first <= ans[ans.size()-1].deadline.begin()->first)
            {
                if(ans[ans.size()-1].chunks.size() < globalB)
                {
                    ans[ans.size()-1].chunks.insert(deadlines[i].chunks[j].second);
                    int remainingTime=ans[ans.size()-1].deadline.begin()->first-deadlines[i].chunks[j].first;
                    ans[ans.size()-1].deadline.erase(ans[ans.size()-1].deadline.begin());
                    ans[ans.size()-1].deadline.insert({remainingTime,i+1});
                    cout<<"End time for chunk "<<deadlines[i].chunks[j].second<<" is:- "<<startime+ogDeadline-remainingTime<<endl;
                }
                else
                {
                    node machine(globalB,globalS);
                    ans.push_back(machine);
                    ans[ans.size()-1].chunks.insert(deadlines[i].chunks[j].second);
                    ans[ans.size()-1].deadline.insert({ogDeadline-deadlines[i].chunks[j].first,i+1});
                    cout<<"End time for chunk "<<deadlines[i].chunks[j].second<<" is:- "<<startime+deadlines[i].chunks[j].first<<endl;
                }
            }
            else
            {
                int endtime=startime;
                ans[ans.size()-1].chunks.insert(deadlines[i].chunks[j].second);
                deadlines[i].chunks[j].first-=ans[ans.size()-1].deadline.begin()->first;
                endtime=max(endtime,startime+ans[ans.size()-1].deadline.begin()->first);
                // ans[ans.size()-1].deadline.erase(ans[ans.size()-1].deadline.begin());
                while(deadlines[i].chunks[j].first>0)
                {
                    node machine(globalB,globalS);
                    ans.push_back(machine);
                    ans[ans.size()-1].deadline.insert({ogDeadline,i+1});
                    ans[ans.size()-1].chunks.insert(deadlines[i].chunks[j].second);
                    if(deadlines[i].chunks[j].first<=ans[ans.size()-1].deadline.begin()->first)
                    {
                        int remainingTime=ans[ans.size()-1].deadline.begin()->first-deadlines[i].chunks[j].first;
                        ans[ans.size()-1].deadline.erase(ans[ans.size()-1].deadline.begin());
                        ans[ans.size()-1].deadline.insert({remainingTime,i+1});
                        endtime=max(endtime,startime+deadlines[i].chunks[j].first);
                        ans[ans.size()-1].chunks.insert(deadlines[i].chunks[j].second);
                        break;
                    }
                    else
                    {   endtime=max(endtime,ans[ans.size()-1].deadline.begin()->first);
                        ans[ans.size()-1].chunks.insert(deadlines[i].chunks[j].second);
                        deadlines[i].chunks[j].first-=ans[ans.size()-1].deadline.begin()->first;
                    }
                }
                cout<<"End time for chunk "<<deadlines[i].chunks[j].second<<" is:- "<<endtime<<endl;
            }
        }
        auto it = ans[ans.size()-1].deadline.begin();
        while (it != ans[ans.size()-1].deadline.end()) {
            if (it->second <= i+1) {
                it = ans[ans.size()-1].deadline.erase(it);
            } else {
                ++it;
            }
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
            d.chunks.push_back({chunk.second,chunk.first});
        }
        sort(d.chunks.begin(),d.chunks.end(),greater<pair<int,int>>());
        deadlines.push_back(d);
    }
    // cout<<"hi";
    // cout<<endl;
    vector<node>ans = firstFit(deadlines);  
    for(int i=0;i<ans.size();i++)
    {
        cout<<"Node "<<i+1<<" Statistics:-\n";
        for(auto x:ans[i].chunks)
        {
            cout<<x<<" ";
        }
        cout<<endl;
        // cout<<"Total number of chunks:- "<<ans[i].totalChunks<<" Last Time for server used is:- "<<ans[i].lastTime<<endl;
    }  
}
