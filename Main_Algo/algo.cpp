#include <bits/stdc++.h>
using namespace std;

int gloabalB,globalS;

struct node
{
    int B,S;
    int slotScheduled=0;
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
    int time=0;
    deadline(int t)
    {
        time = t;
    }
};

bool deadlineComp(deadline a,deadline b)
{
    return a.time<b.time;
}

vector<node> scheduleOneDeadline(deadline& d)
{
    sort(d.chunks.begin(),d.chunks.end(),greater<pair<int,int>>());
    vector<node> ans;
    // cout<<"Deadlines    :\n";
    // for(auto x:d.chunks)
    // {
    //     cout<<x.first<<" "<<x.second<<endl;
    // }
    while(d.chunks.size()&&d.chunks[d.chunks.size()-1].first==0)
    {
        d.chunks.pop_back();
        cout<<"popped out: "<<d.chunks[d.chunks.size()-1].second<<endl;
    }
    
    while(d.chunks.size())
    {
        node machine(gloabalB,globalS);
        vector<pair<int,int>> rem;
        while (d.chunks.size()&&  machine.chunks.size()<gloabalB && machine.slotScheduled<globalS*d.time)
        {
            machine.chunks.insert(d.chunks[d.chunks.size()-1].second);
            if(d.chunks[d.chunks.size()-1].first+machine.slotScheduled<=globalS*d.time)
            {
                if(d.chunks[d.chunks.size()-1].first<=d.time)
                {
                    machine.slotScheduled+=d.chunks[d.chunks.size()-1].first;
                }
                else
                {
                    machine.slotScheduled+=d.time;
                    rem.push_back(d.chunks[d.chunks.size()-1]);
                }
                d.chunks.pop_back();
            }
            else
            {
                if(globalS*d.time-machine.slotScheduled<=d.time)
                {
                    d.chunks[d.chunks.size()-1].first-= globalS*d.time-machine.slotScheduled;
                    machine.slotScheduled=globalS*d.time;
                }
                else
                {
                    d.chunks[d.chunks.size()-1].first-= d.time;
                    machine.slotScheduled+=d.time;
                }
            }
        }
        for(int i=rem.size()-1;i>=0;i--)
        d.chunks.push_back(rem[i]);
        ans.push_back(machine);
    }
    return ans;
}

vector<node> scheduleAll(vector<deadline>& deadlines)
{
    vector<node> ans;
    sort(deadlines.begin(),deadlines.end(),deadlineComp);
    int cnt=1;
    for(int l=0;l<deadlines.size();l++)
    {
        deadline& d = deadlines[l];
        if(ans.size())
        {
            node machine = ans[ans.size()-1];
            vector<pair<int,int>> rem;
            while (d.chunks.size()&&  machine.chunks.size()<gloabalB && machine.slotScheduled<globalS*d.time)
            {
                machine.chunks.insert(d.chunks[d.chunks.size()-1].second);
                if(d.chunks[d.chunks.size()-1].first+machine.slotScheduled<=globalS*d.time)
                {
                    if(d.chunks[d.chunks.size()-1].first<=d.time)
                    {
                        machine.slotScheduled+=d.chunks[d.chunks.size()-1].first;
                    }
                    else
                    {
                        machine.slotScheduled+=d.time;
                        rem.push_back(d.chunks[d.chunks.size()-1]);
                    }
                    d.chunks.pop_back();
                }
                else
                {
                    if(globalS*d.time-machine.slotScheduled<=d.time)
                    {
                        d.chunks[d.chunks.size()-1].first-= globalS*d.time-machine.slotScheduled;
                        machine.slotScheduled=globalS*d.time;
                    }
                    else
                    {
                        d.chunks[d.chunks.size()-1].first-= d.time;
                        machine.slotScheduled+=d.time;
                    }
                }
            }
            for(int i=rem.size()-1;i>=0;i--)
            d.chunks.push_back(rem[i]);
            }
        vector <node> subans = scheduleOneDeadline(d);
        for(int k =0;k<subans.size();k++)
        {
            node& machine = subans[k];
            for(int i =l+1;i<deadlines.size();i++)
            {
                deadline& futuredeadline = deadlines[i];
                for(int j=0;j<futuredeadline.chunks.size();j++)
                {
                    auto& x = futuredeadline.chunks[j];
                    if(machine.chunks.find(x.second)==machine.chunks.end())
                    continue;
                    if(x.first+machine.slotScheduled<=globalS*futuredeadline.time)
                    {
                        machine.slotScheduled+=x.first;
                        x.first=0;
                        cout<<"Here :"<<x.second<<" "<<x.first<<endl;
                    }
                    else if(globalS*futuredeadline.time-machine.slotScheduled>0)
                    {
                        x.first-= globalS*futuredeadline.time-machine.slotScheduled;
                        machine.slotScheduled=globalS*futuredeadline.time;
                    }
                }
            }
            ans.push_back(machine);
        }
        cnt++;
    }
    return ans;
}

int main()
{
    fstream fin("../Test_Cases/testcase.txt");
    int k;
    fin>>gloabalB;
    fin>>globalS;
    fin>>k;
    vector<deadline> deadlines;
    int id,time,n;
    for (int i = 0; i < k; i++)
    {
        fin>>id>>time>>n;
        cout<<time<<endl;
        deadline d(time);
        map<int,int> mp;
        int x;
        while (n--)
        {
            fin>>x;
            mp[x]++;
        }
        for(auto x:mp)
        {
            d.chunks.push_back({x.second,x.first});
            cout<<i<<" "<<x.first<<" "<<x.second<<endl;
        }
        deadlines.push_back(d);
        vector<node> ans = scheduleOneDeadline(d);
        cout<<ans.size()<<endl;
    }
    vector<node> finalAns = scheduleAll(deadlines);
    cout<<finalAns.size()<<endl;
    for(auto x:finalAns)
    {
        for(auto y:x.chunks)
        {
            cout<<y<<" ";
        }
        cout<<endl;
    }
}