#include <bits/stdc++.h>
using namespace std;

vector<string> logs;
queue<string> pending;
map<string, string> wfg;
map<string, bool> visited;
map<string, bool> completed;
vector<string> deadlock;


void printLogs();

struct Event
{
    int linenumber;
    string type;
    string process;
    string sender;
    vector<string> receiverList;
    string receiver;
    string message;
    int timestamp;

    Event(string type, string process, string message, int timestamp)
        : type(type), process(process), message(message), timestamp(timestamp) {}

    Event(string type, string process, string message, string sender, string receiver, int timestamp)
        : type(type), process(process), message(message), sender(sender), receiver(receiver), timestamp(timestamp) {}
};


class LamportClock
{
private:
    map<string, int> clock;

public:
    void incrementClock(string process)
    {
        clock[process]++;
    }

    void update(int time, string process)
    {
        clock[process] = time;
    }

    int getClock(string process)
    {
        return clock[process];
    }
};


class Process
{
private:
    string name;
    vector<Event> events;
    map<string, vector<string>> received;
    int count;
    vector<string> sent;

public:
    Process() {}
    Process(string _name)
    {
        name = _name;
        count = 0;
    }

    int start = 0;
    queue<Event> messageQueue;

    string getName()
    {
        return name;
    }
    int getCount(){
        return count;
    }
    void updateCount(){
        count++;
    }
    void updateMsgQ(Event msgQ)
    {
        messageQueue.push(msgQ);
    }
    void changeQ(queue<Event> q){
        messageQueue = q;
    }
    void updateEvents(vector<Event> _events)
    {

        events = _events;
    }
    void receive(string sender, string msg)
    {
        received[sender].push_back(msg);
    }

    Event getEvent() {
        return events[start];
    }
    vector<string> detectCycle(string name, map<string, bool> &vis, int &res, map<string, Process> &processes, Event event, LamportClock &clock, vector<string> cycle)
    {
        // cout << name << "-->" << wfg[name] << endl;
        vector<string> ret;
        string str = "";
        cycle.push_back(name+ "<" + event.message+">");
        str += name + event.message;
        if (vis[name] == true)
        {
            res = 1; // deadlock
            ret.push_back(name);
            ret.push_back(wfg[name]);
            deadlock = cycle;
            return ret;
        }
        if (completed[wfg[name]] == true)
        {
            bool flag = false;
            queue<Event> que = processes[name].messageQueue;
            queue<Event> modified;
            while (!que.empty())
            {
                Event sentEvent = que.front();
                if (sentEvent.message == event.message && sentEvent.sender == event.sender)
                {
                    flag = true;
                    que.pop();
                    clock.incrementClock(name);
                    event.timestamp = max(clock.getClock(name), sentEvent.timestamp + 1);
                    clock.update(event.timestamp, name);
                    string log = "received " + name + " " + event.message + " " + event.sender + " " + to_string(event.timestamp);
                    logs.push_back(log);
                    if (wfg.find(event.process) != wfg.end())
                    {
                        wfg.erase(event.process);
                        visited[event.process] = false;
                    }
                    processes[name].updateCount();
                    processes[name].start++;
                    break;
                }
                else{
                    modified.push(sentEvent);
                }
                que.pop();
            }
            processes[name].changeQ(modified);
            if (!flag)
            {
                res = 2;
                ret.push_back(wfg[name]);
                ret.push_back(name);
                ret.push_back(event.message);
                return ret;
            }
            else
            {
                
                return ret;
            }
        }
        vis[name] = true;
        Event e = processes[wfg[name]].getEvent();
        return detectCycle(wfg[name], vis, res, processes, e, clock, cycle);
    }
    void execute(LamportClock &clock, map<string, Process> &processes, set<string> &waitingProcesses)
    {
        for (int i = start; i < events.size(); i++)
        {
            Event event = events[i];
            if (event.type == "send")
            {
                clock.incrementClock(name);
                event.timestamp = clock.getClock(name);

                string log = "";
                log += "sent " + event.process + " " + event.message + " (";
                for (auto r : event.receiverList)
                {
                    event.sender = name;
                    if (processes.find(r) == processes.end())
                    {
                        cerr << "Receiver " << r << " does not exist! " << endl;
                        exit(1);
                    }
                    processes[r].updateMsgQ(event);
                    string str = r;
                    str += event.message;
                    sent.push_back(str);
                    log += r+" ";
                }
                log += ") " + to_string(event.timestamp);
                logs.push_back(log);
            }
            else if (event.type == "recv")
            {
                bool flag = false;
                if (processes.find(event.sender) != processes.end())
                {
                    // Simulate message receiving
                    queue<Event> que = messageQueue;
                    while (!que.empty())
                    {
                        Event sentEvent = que.front();
                        if (sentEvent.message == event.message && sentEvent.sender == event.sender)
                        {
                            flag = true;
                            que.pop();
                            clock.incrementClock(name);
                            event.timestamp = max(clock.getClock(name), sentEvent.timestamp + 1);
                            clock.update(event.timestamp, name);
                            string log = "received " + name + " " + event.message + " " + event.sender + " " + to_string(event.timestamp);
                            logs.push_back(log);
                            if (wfg.find(event.process) != wfg.end())
                            {
                                wfg.erase(event.process);
                                visited[event.process] = false;
                            }
                            break;
                        }
                        que.pop();
                    }
                    if (!flag)
                    {
                        start = i;
                        // requireds[name] = event;
                        // if (start != i)
                        if (visited[name] == false)
                        {
                            pending.push(event.process);
                            wfg[name] = event.sender;
                            visited[name] = true;
                        }
                        else
                        {

                            map<string, bool> vis;
                            int res = 3;
                            if (visited[event.sender] || pending.empty())
                            {
                                wfg[event.process] = event.sender;
                                vector<string> rndm;
                                vector<string> nm = detectCycle(name, vis, res, processes, event, clock, rndm);

                                if (res == 1)
                                {
                                    printLogs();
                                    cerr << nm[0] << " is waiting for " <<nm[1]<<" which in turn is waiting. SYSTEM DEADLOCKED!" << endl;
                                    for(int i=0; i<deadlock.size();i++) {
                                        string a = deadlock[i];
                                        cout<<a;
                                        if(i!=deadlock.size()-1){
                                            cout<<"->";
                                        } else {
                                            cout<<endl;
                                        }
                                    }
                                    exit(1);
                                }
                                else if (res == 2)
                                {
                                    printLogs();
                                    cerr << "Appropriate message not sent. " << nm[0] << " disn't send the required message "<<nm[2]<<" to "<<nm[1]<<". SYSTEM STUCK! " << endl;
                                    exit(1);
                                }
                            }
                            pending.push(event.process);
                            wfg[event.process] = event.sender;
                            visited[event.process] = true;
                        }

                        break;
                    }
                }
                else
                {
                    printLogs();
                    cerr << "Error: Process " << event.sender << " not found when receiving message." << endl;
                    exit(1);
                }
            }
            else if (event.type == "print")
            {
                clock.incrementClock(name);
                event.timestamp = clock.getClock(name);
                string log = "printed " + name + " " + event.message + " " + to_string(event.timestamp);
                logs.push_back(log);
            }
            count++;
        }

        if (count == events.size())
        {
            completed[name] = true;
        }
    }
};

void printLogs()
{
    for (auto a : logs)
    {
        cout << a << endl;
    }
}

int main()
{
    map<string, Process> processes;
    LamportClock clock;
    set<string> waitingProcesses;

    string line;
    int lineNumber = 0;
    string processName;

    queue<string> processesQueue;
    vector<Event> events;
    while (getline(cin, line))
    {
        if (line.size() == 0)
        {
            while (!pending.empty())
            {
                string p = pending.front();
                pending.pop();
                processes[p].execute(clock, processes, waitingProcesses);
            }

            printLogs();
            break;
        }
        else
        {
            lineNumber++;
            stringstream ss(line);
            string command;
            ss >> command;
            if (command == "begin")
            {
                string process;
                ss >> process;
                if (process != "process")
                {
                    cerr << "Incorrect input format at line " << lineNumber << endl;
                    exit(1);
                }
                ss >> processName;
                if (!processesQueue.empty())
                {
                    cerr << "Error: Process " << processesQueue.front() << " has not ended." << lineNumber << endl;
                    exit(1);
                }
                if (processes.find(processName) == processes.end())
                {
                    Process p(processName);
                    processesQueue.push(processName);
                    processes[processName] = p;
                    pending.push(processName);
                }
                else
                {
                    cerr << "Error: Process " << processName << " already exists." << lineNumber << endl;
                    exit(1);
                }
            }

            else if (command == "send")
            {
                if (processes.find(processName) != processes.end())

                {
                    Process p = processes[processName];
                    string receivers;
                    ss >> receivers;
                    if (receivers == "")
                    {
                        cerr << "Receivers  cannot be empty !" << lineNumber << endl;
                        exit(1);
                    }
                    if (receivers[0] != '(' || receivers[receivers.size() - 1] != ')')
                    {
                        cerr << "Incorrect input format, brackets inappropriate " << lineNumber << endl;
                        exit(1);
                    }
                    receivers = receivers.substr(1, receivers.size() - 2); // Remove parentheses
                    string message;
                    ss >> message;
                    if (message == "")
                    {
                        cerr << "Message cannot be empty!" << lineNumber << endl;
                        exit(1);
                    }
                    vector<string> receiverList;
                    stringstream receiverStream(receivers);
                    string receiver;
                    while (getline(receiverStream, receiver, ','))
                    {
                        if (receiver == processName)
                        {
                            cerr << "Error: Process " << processName << " sending to itself at line " << lineNumber << "." << endl;
                            exit(1);
                        }
                        receiverList.push_back(receiver);
                        Process r = Process(receiver);
                        r.receive(processName, message);
                    }
                    Event event("send", processName, message, clock.getClock(processName));
                    event.receiverList = receiverList;
                    event.linenumber = lineNumber;
                    events.push_back(event);
                }
                else
                {
                    cerr << "Error: Process does not exist." << lineNumber << endl;
                    exit(1);
                }
            }
            else if (command == "recv")
            {
                if (processes.find(processName) != processes.end())
                {
                    string sender;
                    ss >> sender;
                    string message;
                    ss >> message;
                    Event event("recv", processName, message, sender, processName, clock.getClock(processName));
                    event.linenumber = lineNumber;
                    events.push_back(event);
                }
                else
                {
                    cerr << "Error: Process does not exists." << lineNumber << endl;
                    exit(1);
                }
            }
            else if (command == "print")
            {
                if (processes.find(processName) != processes.end())
                {
                    string message;
                    ss >> message;
                    Event event("print", processName, message, clock.getClock(processName));
                    event.linenumber = lineNumber;
                    events.push_back(event);
                }
                else
                {
                    cerr << "Error: Process does not exists." << lineNumber << endl;
                    exit(1);
                }
            }

            else if (command == "end")
            {
                processes[processName].updateEvents(events);
                events.clear();
                if (processes.find(processName) != processes.end())
                {
                    processesQueue.pop();
                    // processes[processName].printEvents();
                }
                else
                {
                    cerr << "Error: Process " << processName << " not found when ending process on line " << lineNumber << "." << endl;
                    exit(1);
                }
            }

            else
            {
                cerr << "Error: Invalid command on line " << lineNumber << ": " << command << endl;
                exit(1);
            }
        }
    }
    return 0;
}

// begin process p2
// print x1
// recv p1 m1
// print x2
// send (p1) m2
// print x3
// end process
// begin process p1
// send (p2) m1
// print abc
// print def
// end process
//