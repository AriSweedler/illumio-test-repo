//
//  main.cpp
//  illumio-test
//
//  Created by Ari Sweedler on 1/2/18.
//  Copyright © 2018 Ari Sweedler. All rights reserved.
//

#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;

#define NUM_BUCKETS_PURE 105899
#define NUM_BUCKETS_IP 105899
#define NUM_BUCKETS_PORT 105899

//open an OUTPUT file.
//open the FLOW file and read it one line at a time, adding to OUTPUT as we go

/************class IP address**************/
class IP_Address {
    short port;
    short ip[4];
    unsigned long key;
    string stringify(char);
    unsigned long figureKey();
public:
    IP_Address();
    IP_Address(string myLine);
    IP_Address(const IP_Address&);
    unsigned long hash();
    unsigned long getKey();
    string to_string();
    bool onlyPort = false;
    bool onlyIP = false;
};

IP_Address::IP_Address() {
    for(int i = 0; i < 4; i++) {this->ip[i] = 0;}
    this->port = 0;
}

IP_Address::IP_Address(string str)
{
    this->key = 0;
    for(int i = 0; i < 4; i++) {this->ip[i] = 0;}
    //reads in a string, and sets ip[0-3] and port accordingly.
    char c;
    int ip_i = 0;
    int str_i = 0;
    
    if (str.at(0) == '*') {
        this->onlyPort = true;
        str_i++;
    }
    for(; str_i < 16; str_i++) {
        c = str.at(str_i);
//        cout << "\tWe just read in a '" << c << "'." << endl;
        if(c == '.') {
//            cout << "\t\tThat's the end of a number. The number is '" << (int)(this->ip[ip_i]) << "'." << endl;
            ip_i++;
        } else if (c == ':') {
            break;
        } else {
            this->ip[ip_i] *= 10;
            this->ip[ip_i] += (c-'0');
        }
    }
    str_i++;
    if (str.at(str_i) == '*') {
        this->onlyIP = true;
    } else {
        this->port = std::atoi(str.substr(str_i).c_str());
    }
    this->key = figureKey();
}

IP_Address::IP_Address(const IP_Address &ipa) {
    for(int i = 0; i < 4; i++) {
        this->ip[i] = ipa.ip[i];
    }
    this->port = ipa.port;
    this->key = figureKey();
}

unsigned long IP_Address::figureKey() {
    if (this->onlyPort) {return this->port;}
    unsigned long answer = 0;
    for(int i = 0; i < 4; i++) {
        answer += this->ip[i];
        answer <<= 8;
    }
    if (this->onlyIP) {return (answer >> 8);}
    answer += this->port;
    return answer;
}

unsigned long IP_Address::hash() {
    unsigned long answer = this->figureKey();
    return answer % NUM_BUCKETS_PURE;
}

string IP_Address::to_string() {
    //cast the ip chars into numbers
    string a = std::to_string(this->ip[0]);
    string b = std::to_string(this->ip[1]);
    string c = std::to_string(this->ip[2]);
    string d = std::to_string(this->ip[3]);
    string ipstring = this->onlyPort ? "*" : (a + "." + b + "." + c + "." + d);
    string portstring = this->onlyIP ? "*" : (std::to_string(this->port));
    return (ipstring + ":" + portstring);
}

unsigned long IP_Address::getKey() {
    return this->key;
}
/**************************/

/***************class NAT entry***********/
class NAT_Entry {
    NAT_Entry* next;
public:
    NAT_Entry();
    NAT_Entry(string);
    unsigned long hash();
    NAT_Entry* getNext();
    void setNext(NAT_Entry*);
    IP_Address from;
    IP_Address to;
    string to_string();
    void addNext(NAT_Entry*);
};

NAT_Entry::NAT_Entry(string str)
{
    size_t commaIndex = str.find(",");
    this->from = IP_Address(str.substr(0, commaIndex));
    this->to = IP_Address(str.substr(commaIndex+1));
    this->next = nullptr;
}

unsigned long NAT_Entry::hash() {
    return this->from.hash();
}

NAT_Entry* NAT_Entry::getNext() {
    return this->next;
}

void NAT_Entry::setNext(NAT_Entry* other) {
    this->next = other;
}

string NAT_Entry::to_string() {
    return this->from.to_string() + " --> " + this->to.to_string();
}

void NAT_Entry::addNext(NAT_Entry* ptr) {
    this->next = ptr;
}

/**************************/


/************MAIN**************/
NAT_Entry* NAT_Database_pure[NUM_BUCKETS_PURE];
NAT_Entry* NAT_Database_ip[NUM_BUCKETS_IP];
NAT_Entry* NAT_Database_port[NUM_BUCKETS_PORT];

int myOpen (string name, ifstream* fileptr);
NAT_Entry* checkMatch(NAT_Entry* ent, IP_Address* ip);

int main () {
    for(int i = 0; i < NUM_BUCKETS_PURE; i++) {
        NAT_Database_pure[i] = NULL;
    }
    
    //open the NAT file and read in the data
    ifstream nat_file;
    string myLine;
    nat_file.open("NAT", fstream::in);
    if (!nat_file) {cout << "Error opening NATfile." << endl; exit(1);}

    while (nat_file >> myLine) {
        NAT_Entry* myEntry = new NAT_Entry(myLine);
        NAT_Entry* other;
        if (myEntry->from.onlyIP) {
            other = NAT_Database_ip[myEntry->from.hash()];
            NAT_Database_ip[myEntry->from.hash()] = myEntry;
        } else if (myEntry->from.onlyPort) {
            other = NAT_Database_port[myEntry->from.hash()];
            NAT_Database_port[myEntry->from.hash()] = myEntry;
        } else {
            other = NAT_Database_pure[myEntry->from.hash()];
            NAT_Database_pure[myEntry->from.hash()] = myEntry;
        }
        myEntry->setNext(other);
    }
    nat_file.close();
    cout << endl;
    
    ifstream flow_file;
    ofstream out_file;
    flow_file.open("FLOW", fstream::in);
    out_file.open("OUTPUT", fstream::out);
    if (!flow_file) {cout << "Error opening FLOWfile." << endl; exit(1);}
    if (!out_file) {cout << "Error opening OUTfile." << endl; exit(1);}
    while (getline(flow_file, myLine)) {
        IP_Address* myIP = new IP_Address(myLine);
        NAT_Entry* answer = NULL;
        if (myIP->onlyIP) {
            answer = checkMatch(NAT_Database_ip[myIP->hash()], myIP);
        } else if (myIP->onlyPort) {
            answer = checkMatch(NAT_Database_port[myIP->hash()], myIP);
        } else {
            answer = checkMatch(NAT_Database_pure[myIP->hash()], myIP);
            
            if (answer == NULL) {
                myIP->onlyIP = true;
                answer = checkMatch(NAT_Database_ip[myIP->hash()], myIP);
                myIP->onlyIP = false;
            }
            if (answer == NULL) {
                myIP->onlyPort = true;
                answer = checkMatch(NAT_Database_port[myIP->hash()], myIP);
                myIP->onlyPort = false;
            }
        }
        
        if (answer == NULL) {
            out_file << "No nat match for " << myIP->to_string() << endl;
        } else {
            out_file << myIP->to_string() << " -> " << answer->to.to_string() << endl;
        }
    }
    flow_file.close();
    out_file.close();

    return 0;
}

NAT_Entry* checkMatch(NAT_Entry* ent, IP_Address* ip) {
    while (ent != NULL) {
        if ((ip->getKey() == ent->from.getKey()) ||
            (ent->from.onlyPort && ip->onlyPort) ||
            (ent->from.onlyIP && ip->onlyIP)) {
            return ent;
        } else {
            ent = ent->getNext();
        }
    }
    return NULL;
}


int myOpen (string name, ifstream* fileptr)
{
    fileptr->open(name);
    if (!(*fileptr)) {
        cerr << "Error: Unable to open file '" << name << "'." << endl;
        exit(1);
    }
    return 0;
}
/**************************/
