#include<bits/stdc++.h>
using namespace std;

map<int,map<int,int>>chunk_ts;
map<int,int>slots_scheduled;
map<int,vector<int>>machines_to_chunks;
map<int,map<int,map<int,int>>>F; //F(c,n,i)
int b;

// comparison function to sort based on number of time slots left
static bool compareByTimeSlotsLeft(const int& chunk1, const int& chunk2, const int d) {
    // Get the number of time slots left for each chunk
    int timeSlotsLeft1 = chunk_ts[chunk1][d];
    int timeSlotsLeft2 = chunk_ts[chunk2][d];
    // Sort in ascending order of time slots left
    return min(timeSlotsLeft1,d) < min(timeSlotsLeft2,d);
}

pair<int,pair<int,int>>find_Hb_indices(deque<int>&q, int B, int S, int d)
{
    int i=0; int j=0;
    int sum = 0;
    while(j<q.size())
    {
        if(j-i+1<B){
            sum+=min(chunk_ts[q[j]][d],d);
            j++;
        }
        else{
            sum+=min(chunk_ts[q[j]][d],d);
            if(sum>=S*d) return {sum,{i,j}};
            sum-=min(chunk_ts[q[i]][d],d); i++; j++;
        }
    }
    return {sum,{i,j}};
}

void schedule(deque<int>&q, int s, int e, int S, int d, int m, bool pass1=true)
{
    e = min<int>(e,q.size());
    sort(q.begin()+s, q.begin()+e, [&](const int& chunk1, const int& chunk2) {
            return compareByTimeSlotsLeft(chunk1, chunk2, d);
    });
    while(s<=e && q.size() && b>0)
    {
        // Minimum is taken to ensure that no two VMs can access the same chunk in a given time slot
        if(S*d-min(chunk_ts[q[s]][d],d)-slots_scheduled[m]>=0)
        {
            slots_scheduled[m]+=min(chunk_ts[q[s]][d],d);
            F[q[s]][m][d]+=min(chunk_ts[q[s]][d],d); //To get the final schedule
            chunk_ts[q[s]][d] -= min(chunk_ts[q[s]][d],d); 
            if(pass1)
            {
                machines_to_chunks[m].push_back(q[s]);
                b--;
            }
            if(chunk_ts[q[s]][d]==0)
            {
                q.erase(q.begin()+s);
                e--;
            }
            else break;
            
        }
        else{
            chunk_ts[q[s]][d]-= min((S*d-slots_scheduled[m]),d);
            F[q[s]][m][d]+=min((S*d-slots_scheduled[m]),d);
            slots_scheduled[m] += min((S*d-slots_scheduled[m]),d); // Slots scheduled are over
            if(pass1)
            {
                machines_to_chunks[m].push_back(q[s]);
                b--;
            }
            break;
        }
    }

}

int main()
{
    int B; cin>>B;
    int S; cin>>S;
    int K; cin>>K;
    map<int,set<int>>deadline_to_chunks;
    map<int,deque<int>>deadline_chunks;
    set<int>deadlines;
    for(int _=0; _<K; _++)
    {
        int j; cin>>j;
        int d; cin>>d;
        int nc; cin>>nc;
        deadlines.insert(d);
        for(int i=0; i<nc; i++)
        {
            int chunk_id; cin>>chunk_id;
            chunk_ts[chunk_id][d]++;
            deadline_to_chunks[d].insert(chunk_id);
        }
    }
    for(auto x: deadline_to_chunks)
    {
        for(auto s: x.second)
        {
            deadline_chunks[x.first].push_back(s);
        }
    }

    cout << "chunk_ts:" << endl;
    for (const auto& chunk : chunk_ts) {
        cout << "Chunk ID: " << chunk.first << endl;
        for (const auto& deadline : chunk.second) {
            cout << "  Deadline " << deadline.first << ": " << deadline.second << " time slots needed" << endl;
        }
    }

    //Phase 1
    int m = 0; // Starting with 1 machine
    b = B; // Globally b slots are available
    for(auto it = deadlines.begin(); it!=deadlines.end(); it++)
    {
        vector<int>machines_scheduled;
        //Sort based on deadline
        int d = *it;
        deque<int>v = deadline_chunks[d];
        while(!v.empty())
        {
            sort(v.begin(), v.end(), [&](const int& chunk1, const int& chunk2) {
            return compareByTimeSlotsLeft(chunk1, chunk2, d);
            });

            while(v.size() && chunk_ts[v.front()][d]==0) v.pop_front();

            int sum = 0; int lim = B;
            for(int i=v.size()-1; i>=0 && lim>0; i--)
            {
                sum+=min(chunk_ts[v[i]][d],d); lim--;
            }
            if(sum>S*d)
            {
                m++; b=B; //Create a new Machine
                machines_scheduled.push_back(m);
                pair<int,pair<int,int>>idx = find_Hb_indices(v,B,S,d);
                int start = idx.second.first;
                int end = idx.second.second;
                //Schedule the chunks in node
                schedule(v,start,end,S,d,m);
                while(b>0)
                {
                    cout<<"B_rem: "<<b<<endl;
                    schedule(v,start,end,S,d,m);
                }
            }
            else break;
        }

        while(!v.empty())
        {
            m++; b=B; //Create a new Machine
            machines_scheduled.push_back(m);
            schedule(v,0,B-1,S,d,m);
            while(b>0)
            {
                cout<<"B_rem: "<<b<<endl;
                schedule(v,0,B-1,S,d,m);
            }
        }

        //Phase 2
        auto next_deadline = it;
        next_deadline++;
        //auto it1=next_deadline; it1!=deadlines.end();it1++
        for(auto machines:machines_scheduled)
        {
            //Get list of chunks scheduled in this machine
            for(auto it1=next_deadline; it1!=deadlines.end();it1++)
            {
                int d1 = *it1;
                deque<int>q;
                for(auto chunks : machines_to_chunks[machines]){ 
                    if(chunk_ts[chunks][d1] >0)
                    q.push_back(chunks);
                }

                if(q.empty())continue;

                schedule(q,0,q.size()-1,S,d1,machines,false);
            }
            
        }

    }

    cout<<endl;
    cout << "machines_to_chunks:" << endl;
    for (const auto& pair : machines_to_chunks) {
        cout << "Machine " << pair.first << ": ";
        for (int chunk : pair.second) {
            cout << chunk << " ";
        }
        cout << endl;
    }

    cout<<endl<<"scheduling of chunks"<<endl;
    for(auto it : F) {
        int chunk_id = it.first;
        cout<<"****************"<<endl;
        cout << "Chunk id: " << chunk_id << endl;
        for(auto it1 : it.second) {
            int machine_id = it1.first;
            cout << "Time slots scheduled in machine: " << machine_id << " before deadline ";
            for(auto it2 : it1.second) {
                int deadline = it2.first;
                int value = it2.second;
                cout << deadline << " is: " << value << endl;
            }
        }
    }

}