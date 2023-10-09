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



// #include <bits/stdc++.h>
// using namespace std;

// vector<string> logs;
// struct Event
// {
//     int linenumber;
//     string type;
//     string process;
//     string sender;
//     vector<string> receiverList;
//     string receiver;
//     string message;
//     int timestamp;

//     Event(string type, string process, string message, int timestamp)
//         : type(type), process(process), message(message), timestamp(timestamp) {}

//     Event(string type, string process, string message, string sender, string receiver, int timestamp)
//         : type(type), process(process), message(message), sender(sender), receiver(receiver), timestamp(timestamp) {}
// };

// class LamportClock
// {
// private:
//     map<string, int> clock;

// public:
//     void incrementClock(string process)
//     {
//         clock[process]++;
//     }

//     int getClock(string process)
//     {
//         return clock[process];
//     }
// };

// class Process
// {
// private:
//     string name;
//     vector<Event> events;
//     queue<Event> messageQueue;

// public:
//     Process() {}
//     Process(string _name)
//     {
//         name = _name;
//     }

//     void execute(Event event, LamportClock &clock, map<string, Process> &processes, set<string> &waitingProcesses)
//     {
//         if (event.type == "send")
//         {
//             clock.incrementClock(name);
//             event.timestamp = clock.getClock(name);
//             messageQueue.push(event);
//             string log = "";
//             log+= "sent "+event.process+" "+event.message+" (";
//             for(auto r: event.receiverList){
//                 log += r;
//             }
//             log+=") "+ event.timestamp;
//         }
//         else if (event.type == "recv")
//         {
//             if (processes.find(event.sender) != processes.end())
//             {
//                 // Simulate message receiving
//                 while (!messageQueue.empty())
//                 {
//                     Event sentEvent = messageQueue.front();
//                     if (sentEvent.message == event.message && sentEvent.sender == event.sender)
//                     {
//                         messageQueue.pop();
//                         clock.incrementClock(name);
//                         event.timestamp = max(clock.getClock(name), sentEvent.timestamp) + 1;
//                         break;
//                     }
//                     messageQueue.pop();
//                 }
//             }
//             else
//             {
//                 cerr << "Error: Process " << event.sender << " not found when receiving message." << endl;
//                 exit(1);
//             }
//         }
//         else if (event.type == "print")
//         {
//             clock.incrementClock(name);
//             event.timestamp = clock.getClock(name);
//         }
//         events.push_back(event);

//         // Check for deadlock
//         // if (event.type == "recv")
//         // {
//         //     waitingProcesses.erase(event.process);
//         // }
//         // else if (event.type == "send")
//         // {
//         //     for (const string &receiver : event.receiverList)
//         //     {
//         //         waitingProcesses.insert(receiver);
//         //     }
//         // }
//         // if (waitingProcesses.empty())
//         // {
//         //     printEvents();
//         //     cerr << "Error: Deadlock detected." << endl;
//         //     exit(1);
//         // }
//     }

//     void printEvents()
//     {
//         for (const Event &event : events)
//         {
//             if (event.type == "send")
//             {
//                 cout << "sent " << event.process << " " << event.message << " (" << event.receiver << ") " << event.timestamp << endl;
//             }
//             else if (event.type == "recv")
//             {
//                 cout << "received " << event.process << " " << event.message << " " << event.sender << " " << event.timestamp << endl;
//             }
//             else if (event.type == "print")
//             {
//                 cout << "printed " << event.process << " " << event.message << " " << event.timestamp << endl;
//             }
//         }
//     }
// };

// int main()
// {
//     map<string, Process> processes;
//     LamportClock clock;
//     set<string> waitingProcesses;

//     string line;
//     int lineNumber = 0;
//     vector<Event> events;
//     string processName;

//     queue<string> processesQueue;

//     while (getline(cin, line))
//     {
//         if (line.size() == 0)
//         {

//             for (auto e : events)
//             {
//                 cout << "line: " << e.linenumber << endl;
//                 cout << "type: " << e.type << endl;
//                 cout << "process: " << e.process << endl;
//                 cout << "sender: " << e.sender << endl;
//                 cout << "receiver: " << e.receiver << endl;
//                 cout << "message: " << e.message << endl;
//                 cout << "timestamp: " << e.timestamp << endl;
//                 break;
//             }

//             for (auto event : events)
//             {
//                 if (event.type == "send")
//                 {
//                     string _process = event.process;
//                     for (const string &receiver : event.receiverList)
//                     {
//                         if (processes.find(receiver) != processes.end())
//                         {
//                             event.receiver = receiver;
//                             processes[processName].execute(event, clock, processes, waitingProcesses);
//                         }
//                         else
//                         {
//                             cerr << "Error: Receiver process " << receiver << " not found for executing command at: " << event.linenumber << "." << endl;
//                             exit(1);
//                         }
//                     }
//                 }
//                 else if(event.type == "recv") {
//                     processes[processName].execute(event, clock, processes, waitingProcesses);
//                 }
//                 else if(event.type == "print") {
//                     processes[processName].execute(event, clock, processes, waitingProcesses);
//                 }
//             }
//         }
//         else
//         {
//             lineNumber++;
//             stringstream ss(line);
//             string command;
//             ss >> command;
//             if (command == "begin")
//             {
//                 string process;
//                 ss >> process;
//                 ss >> processName;
//                 if (!processesQueue.empty())
//                 {
//                     cerr << "Error: Process " << processesQueue.front() << " has not ended." << endl;
//                     exit(1);
//                 }
//                 if (processes.find(processName) == processes.end())
//                 {
//                     Process p(processName);
//                     processesQueue.push(processName);
//                     processes[processName] = p;
//                 }
//                 else
//                 {
//                     cerr << "Error: Process " << processName << " already exists." << endl;
//                     exit(1);
//                 }
//             }

//             else if (command == "send")
//             {
//                 if (processes.find(processName) != processes.end())
//                 {
//                     string receivers;
//                     ss >> receivers;
//                     cout << "receivers: " << receivers << endl;
//                     cout << receivers.size();
//                     receivers = receivers.substr(1, receivers.size() - 2); // Remove parentheses
//                     string message;
//                     ss >> message;
//                     vector<string> receiverList;
//                     stringstream receiverStream(receivers);
//                     string receiver;
//                     while (getline(receiverStream, receiver, ','))
//                     {
//                         if (receiver == processName)
//                         {
//                             cerr << "Error: Process " << processName << " sending to itself at line " << lineNumber << "." << endl;
//                             exit(1);
//                         }
//                         receiverList.push_back(receiver);
//                     }
//                     Event event("send", processName, message, clock.getClock(processName));
//                     event.receiverList = receiverList;
//                     event.linenumber = lineNumber;
//                     events.push_back(event);
//                 }
//                 else
//                 {
//                     cerr << "Error: Process " << processName << " already exists." << endl;
//                     exit(1);
//                 }
//             }
//             else if (command == "recv")
//             {
//                 if (processes.find(processName) != processes.end())
//                 {
//                     string sender;
//                     ss >> sender;
//                     string message;
//                     ss >> message;
//                     Event event("recv", processName, message, sender, processName, clock.getClock(processName));
//                     event.linenumber = lineNumber;
//                     events.push_back(event);
//                 }
//                 else
//                 {
//                     cerr << "Error: Process " << processName << " already exists." << endl;
//                     exit(1);
//                 }
//             }
//             else if (command == "print")
//             {
//                 if (processes.find(processName) != processes.end())
//                 {
//                     string message;
//                     ss >> message;
//                     Event event("print", processName, message, clock.getClock(processName));
//                     event.linenumber = lineNumber;
//                     events.push_back(event);
//                 }
//                 else
//                 {
//                     cerr << "Error: Process " << processName << " already exists." << endl;
//                     exit(1);
//                 }
//             }

//             else if (command == "end")
//             {
//                 if (processes.find(processName) != processes.end())
//                 {
//                     processesQueue.pop();
//                     // processes[processName].printEvents();
//                 }
//                 else
//                 {
//                     cerr << "Error: Process " << processName << " not found when ending process on line " << lineNumber << "." << endl;
//                     exit(1);
//                 }
//             }

//             else
//             {
//                 cerr << "Error: Invalid command on line " << lineNumber << ": " << command << endl;
//                 exit(1);
//             }
//         }
//     }
//     // while (getline(cin, line)) {
//     //     cout<<line;
//     //     lineNumber++;
//     //     stringstream ss(line);
//     //     string command;
//     //     ss >> command;
//     //     string processName;

//     //     if (command == "begin") {
//     //         string process;
//     //         ss >> process;
//     //         ss >> processName;
//     //         cout<<processName<<endl;

//     //     }
//     //     else if (command == "send" || command == "recv" || command == "print") {
//     //         // string processName;
//     //         // ss >> processName;
//     //         // string message;
//     //         // ss >> message;
//     //         // processName = processName.substr(0, processName.size() - 1);  // Remove trailing comma
//     //         cout<<"process: "<<processName<<endl;

//     //         if (processes.find(processName) != processes.end()) {
//     //             if (command == "send") {
//     //                 string receivers;
//     //                 ss >> receivers;
//     //                 cout<<"receivers: "<<receivers<<endl;
//     //                 cout<<receivers.size();
//     //                 receivers = receivers.substr(1, receivers.size() - 2);  // Remove parentheses
//     //                 string message;
//     //                 ss >> message;
//     //                 vector<string> receiverList;
//     //                 stringstream receiverStream(receivers);
//     //                 string receiver;
//     //                 while (getline(receiverStream, receiver, ',')) {
//     //                     receiverList.push_back(receiver);
//     //                 }
//     //                 Event event("send", processName, message, clock.getClock(processName));
//     //                 event.receiverList = receiverList;  // Store the receiver list for deadlock detection
//     //                 for (const string& receiver : receiverList) {
//     //                     if (processes.find(receiver) != processes.end()) {
//     //                         event.receiver = receiver;
//     //                         processes[processName].execute(event, clock, processes, waitingProcesses);
//     //                     }
//     //                     else {
//     //                         cerr << "Error: Receiver process " << receiver << " not found." << endl;
//     //                         exit(1);
//     //                     }
//     //                 }
//     //             }
//     //             else if (command == "recv") {
//     //                 string sender;
//     //                 ss >> sender;
//     //                 string message;
//     //                 ss >> message;
//     //                 Event event("recv", processName, message, sender, processName, clock.getClock(processName));
//     //                 processes[processName].execute(event, clock, processes, waitingProcesses);
//     //             }
//     //             else if (command == "print") {
//     //                 string message;
//     //                 ss >> message;
//     //                 Event event("print", processName, message, clock.getClock(processName));
//     //                 processes[processName].execute(event, clock, processes, waitingProcesses);
//     //             }
//     //         }
//     //         else {
//     //             cerr << "Error: Process " << processName << " not found when processing event on line " << lineNumber << "." << endl;
//     //             exit(1);
//     //         }
//     //     }
//     //     else if (command == "end") {
//     //         if (processes.find(processName) != processes.end()) {
//     //             processes[processName].printEvents();
//     //         }
//     //         else {
//     //             cerr << "Error: Process " << processName << " not found when ending process on line " << lineNumber << "." << endl;
//     //             exit(1);
//     //         }
//     //     }
//     //     else {
//     //         cerr << "Error: Invalid command on line " << lineNumber << ": " << command << endl;
//     //         exit(1);
//     //     }
//     // }

//     return 0;
// }




