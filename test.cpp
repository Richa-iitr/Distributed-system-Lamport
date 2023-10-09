#include <bits/stdc++.h>
using namespace std;


vector<string> logs;
queue<string> pending;
map<string,string> wfg;
map<string,bool> visited;

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
    queue<Event> messageQueue;
    map<string, vector<string>> received;

public:
    Process() {}
    Process(string _name)
    {
        name = _name;
    }

    int start = 0;

    string getName()
    {
        return name;
    }
    void updateMsgQ(Event msgQ)
    {
        messageQueue.push(msgQ);
    }
    void updateEvents(vector<Event> _events)
    {

        events = _events;
    }
    void receive(string sender, string msg)
    {
        received[sender].push_back(msg);
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
                    if(processes.find(r) == processes.end()) {
                        cerr<<"Receiver "<<r<<" does not exist! "<< endl;
                        exit(1);
                    }
                    processes[r].updateMsgQ(event);
                    log += r;
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
                            event.timestamp = max(clock.getClock(name), sentEvent.timestamp+1);
                            clock.update(event.timestamp, name);
                            string log = "received " + name + " " + event.message + " " + event.sender + " " + to_string(event.timestamp);
                            logs.push_back(log);
                            if(wfg.find(event.process) != wfg.end()){
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
                        if (pending.empty())
                        {
                            string name = event.process;
                            printLogs();
                            cerr << name << " received msg before it is sent | SENDER DIDN'T SEND" << endl;
                            exit(1);
                        }
                        if(visited[event.sender] == true) {
                            string name = event.process;
                            printLogs();
                            cerr << "system deadlocked, " <<event.sender<<" is waiting and is waited for"<< endl;
                            exit(1);
                        }
                        pending.push(event.process);
                        wfg[event.process] = event.sender;
                        visited[event.process] = true;
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
            while (!pending.empty()){
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
                    cerr << "Error: Process " << processesQueue.front() << " has not ended." <<lineNumber<< endl;
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
                    cerr << "Error: Process " << processName << " already exists." <<lineNumber<< endl;
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