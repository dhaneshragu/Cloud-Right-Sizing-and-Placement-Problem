#include<bits/stdc++.h>
using namespace std;

map<int,map<int,int>>chunk_ts;
map<int,int>slots_scheduled;
map<int,vector<int>>machines_to_chunks;
map<int,map<int,map<int,int>>>F; //F(c,n,i)
map<int,set<int>>deadline_to_chunks;
map<int,deque<int>>deadline_chunks;
set<int>deadlines;
map<int,map<int,int>>num_slots_sofar; // num[chunk][machine]
int B;
int S;
int K;   
int b;

// comparison function to sort based on number of time slots left
static bool compareByTimeSlotsLeft(const int& chunk1, const int& chunk2, const int d, const int m) {
    // Get the number of time slots left for each chunk
    int timeSlotsLeft1 = chunk_ts[chunk1][d];
    int timeSlotsLeft2 = chunk_ts[chunk2][d];
    // Sort in ascending order of time slots left
    return min(timeSlotsLeft1,d-num_slots_sofar[chunk1][m]) < min(timeSlotsLeft2,d-num_slots_sofar[chunk2][m]);
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

// Pass 1 tells if its scheduling for a new deadline ( check B constraint, add to machines) or we are trying to satisfy remaining deadlines of that job (Optimising)
void schedule(deque<int>&q, int s, int e, int S, int d, int m, bool pass1=true)
{
    e = min<int>(e,q.size()-1); // sort from s to e. This is an edge case for when e is very large than size of queue
    sort(q.begin()+s, q.begin()+e, [&](const int& chunk1, const int& chunk2) {
            return compareByTimeSlotsLeft(chunk1, chunk2, d,m);
    });

    // If pass1, then check for b constraint, otherwise no
    while(s<=e && q.size() && (pass1 ? b>0 : true))
    {
        // Minimum is taken to ensure that no two VMs can access the same chunk in a given time slot
        //TODO: take previous deadline for the upper limit
        if(S*d-min(chunk_ts[q[s]][d],d-num_slots_sofar[q[s]][m])-slots_scheduled[m]>=0)
        {
        
            slots_scheduled[m]+=min(chunk_ts[q[s]][d],d-num_slots_sofar[q[s]][m]); // Get the number of slots scheduled for that machine
            F[q[s]][m][d]+=min(chunk_ts[q[s]][d],d-num_slots_sofar[q[s]][m]); //To get the final schedule
            chunk_ts[q[s]][d] -= min(chunk_ts[q[s]][d],d-num_slots_sofar[q[s]][m]);  // Decrease the required ts for a chunk
            num_slots_sofar[q[s]][m]+=min(chunk_ts[q[s]][d],d-num_slots_sofar[q[s]][m]);
            if(pass1) // If its pass 1, we are assigning this chunk new, so decrease b and store it in machine to chunks
            {
                machines_to_chunks[m].push_back(q[s]);
                b--;
            }
            if(chunk_ts[q[s]][d]==0) // If all the timeslots are over(normal case)
            {
                q.erase(q.begin()+s);
                e--;
            }
            else{ // For the special case, when min(,d) is considered, move to next one
                s++;
            }
            
        }
        else{ // When all slots cant be scheduled
            chunk_ts[q[s]][d]-= min((S*d-slots_scheduled[m]),d-num_slots_sofar[q[s]][m]); // Decrement the time slot required for that chunk
            F[q[s]][m][d]+=min((S*d-slots_scheduled[m]),d-num_slots_sofar[q[s]][m]); // Get the information of how many slots scheduled where for this chunk
            slots_scheduled[m] += min((S*d-slots_scheduled[m]),d-num_slots_sofar[q[s]][m]); //number of slots scheduled in that machine
            num_slots_sofar[q[s]][m]+=min((S*d-slots_scheduled[m]),d-num_slots_sofar[q[s]][m]);
            if(pass1)
            {
                machines_to_chunks[m].push_back(q[s]);
                b--;
            }
            // When all slots are scheduled, break, otherwise try to schedule the next node
            if(slots_scheduled[m]==S*d)
            break;
            else
            s++;
        }
    }

}

void ScheduleVMs(int machine_id)
{
    cout<<"Proper Schedule in machine : "<<machine_id<<endl;
    map<int,int> timeslots;
    map<int,int> lastscheduled;
    auto maxdealine = deadlines.rbegin();
    vector<vector<int>> VMs (S, vector<int> (*maxdealine,0));
    for(auto it:machines_to_chunks[machine_id])
        lastscheduled[it]=*maxdealine;
    for (auto rit = deadlines.rbegin(); rit != deadlines.rend(); rit++) 
    {
        int d = *rit;
        for(auto it:machines_to_chunks[machine_id])
        {
            int slots = F[it][machine_id][d];
            // cout<<"Chunk    :"<<it<<"   Deadline   :"<<d<<"     Slots :"<<slots<<endl;
            int i =1;
            int skip=0;
            d = min(d,lastscheduled[it]);
            while(i<=slots)
            {
                int Vmid = timeslots[d-i-skip];
                if(Vmid>=S)
                {
                    skip++;
                    continue;
                }
                VMs[Vmid][d-i-skip]=it;
                lastscheduled[it] = d-i-skip;
                timeslots[d-i-skip]++;
                i++;
            }
        }
    }
    // cout<<"Slots            :   ";
    // for(int i=0;i<*maxdealine;i++)
    // {
    //     cout<<i<<"  ";
    // }
    // cout<<endl;
    for(int i =0;i<S;i++)
    {
        cout<<"Scheudle for VM "<<i+1<<" :  ";
        for(int j = 0;j<*maxdealine;j++)
        {
            cout<<"Slot "<<j<<": "<<VMs[i][j]<<"  ";
        }
        cout<<endl;
    }
}

int main()
{
    cin>>B;
    cin>>S;
    cin>>K;
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
        set<int>machines_scheduled; // To store the new machines that have been created for this deadline
        int d = *it;
        deque<int>v = deadline_chunks[d];
        while(!v.empty())
        {
            //Sort based on timeslots required for that deadline
            sort(v.begin(), v.end(), [&](const int& chunk1, const int& chunk2) {
            return compareByTimeSlotsLeft(chunk1, chunk2, d,m);
            });
            
            // Dont count the ones that are already finished
            while(v.size() && chunk_ts[v.front()][d]==0) v.pop_front();

            // While popping out, it might become empty, so break here
            if(v.empty()) break;

            // Finds the first B length window where sum is >= S*d, have used sliding window concept here for optimisation
            pair<int,pair<int,int>>idx = find_Hb_indices(v,B,S,d);
            int start = idx.second.first;
            int end = idx.second.second;

            // Special optimisation case to reuse the free chunk slot in previous machine
            while(b>0 && b!=B && S*d-slots_scheduled[m]>0 && v.size()) //Some slot of previous machine is left to be filled
            {
                machines_scheduled.insert(m);
                schedule(v,start,end,S,d,m); // Schedule these nodes in that
            }

            // this is to check the sum of largest B chunks. If its <=S*d then we can schedule any of B chunks in VMs, otherwise we have to use the Hb indices
            int sum = 0; int lim = B;
            for(int i=v.size()-1; i>=0 && lim>0; i--)
            {
                sum+=min(chunk_ts[v[i]][d],d-num_slots_sofar[v[i]][m]); lim--;
            }

            // Make a new machine and schedule in that
            if(sum>S*d)
            {
                m++; b=B; //Create a new Machine
                machines_scheduled.insert(m);
                //Schedule the chunks in node
                schedule(v,start,end,S,d,m);
            }
            else break;
        }

        // When sum <=S*d case, schedule B chunks at once here
        while(!v.empty())
        {
            m++; b=B; //Create a new Machine
            machines_scheduled.insert(m);
            schedule(v,0,B-1,S,d,m);
        }

        //Phase 2
        auto next_deadline = it;
        next_deadline++;

        for(auto machines:machines_scheduled)
        {
            // Go to all deadlines after current
            for(auto it1=next_deadline; it1!=deadlines.end();it1++)
            {
                int d1 = *it1;
                deque<int>q;

                //Get list of chunks scheduled in this machine
                for(auto chunks : machines_to_chunks[machines]){ 
                    if(chunk_ts[chunks][d1] >0)
                    q.push_back(chunks);
                }

                if(q.empty())continue;
                // Here we dont have to count the machines or decrease the slots, so made phase1 as false
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
    for (const auto& pair : machines_to_chunks) {
        ScheduleVMs(pair.first);
        cout << endl;
    }

}