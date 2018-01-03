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

#define NUM_BUCKETS 256

//open an OUTPUT file.
//open the FLOW file and read it one line at a time, adding to OUTPUT as we go

/************class IP address**************/
class IP_Address {
    short port;
    char ip[4];
    bool ipStar = false;
    bool portStar = false;
    string stringify(char);
public:
    IP_Address();
    IP_Address(string myLine);
    IP_Address(const IP_Address&);
    unsigned long hash();
    unsigned long key();
    string to_string();
};

IP_Address::IP_Address() {
    for(int i = 0; i < 4; i++) {this->ip[i] = 0;}
    this->port = 0;
}

IP_Address::IP_Address(string str)
{
    //reads in a string, and sets ip[0-3] and port accordingly.
    char c;
    int ip_i = 0;
    int str_i = 0;
    
    if (str.at(0) == '*') {
        this->ipStar = true;
        str_i++;
    }
    for(; str_i < 16; str_i++) {
        c = str.at(str_i);
        if(c == '.') {
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
        this->portStar = true;
    } else {
        this->port = std::atoi(str.substr(str_i).c_str());
    }
}

IP_Address::IP_Address(const IP_Address &ipa) {
    for(int i = 0; i < 4; i++) {
        this->ip[i] = ipa.ip[i];
    }
    this->port = ipa.port;
}

//we could get a better hash function here
unsigned long IP_Address::hash() {
    unsigned long answer = 0;
    for(int i = 0; i < 4; i++) {
        answer += this->ip[i];
        answer <<= 8;
    }
    answer += this->port;
    return answer%NUM_BUCKETS;
}

unsigned long IP_Address::key() {
    unsigned long answer = 0;
    for(int i = 0; i < 4; i++) {
        answer += this->ip[i] + 7;
        answer <<= 8;
    }
    answer += this->port;
    return answer;
}

string IP_Address::to_string() {
    //cast the ip chars into numbers
    string a = std::to_string(this->ip[0]);
    string b = std::to_string(this->ip[1]);
    string c = std::to_string(this->ip[2]);
    string d = std::to_string(this->ip[3]);
    string e = std::to_string(this->port);
    return (a + "." + b + "." + c + "." + d + ":" + e);
}
/**************************/

/***************class NAT entry***********/
class NAT_Entry {
    IP_Address from;
    IP_Address to;
    NAT_Entry* next;
public:
    NAT_Entry();
    NAT_Entry(string);
    unsigned long hash();
    unsigned long key();
    NAT_Entry* getNext();
    IP_Address getTo();
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

unsigned long NAT_Entry::key() {
    return this->from.key();
}

NAT_Entry* NAT_Entry::getNext() {
    return this->next;
}

IP_Address NAT_Entry::getTo() {
    return this->to;
}

/**************************/


/************MAIN**************/
NAT_Entry* NAT_Database[NUM_BUCKETS];
int myOpen (string name, ifstream* fileptr);
void addToOutput(string);

int main () {
    IP_Address myIP = IP_Address("1.2.3.4:10\n");
    cout << "My starting IP address is " << myIP.to_string() << endl;
    
    //open the NAT file and read in the data
    ifstream file;
    string myLine;
    myOpen("./NAT", &file);
    while (getline(file, myLine)) {
        NAT_Entry myEntry = NAT_Entry(myLine);
        NAT_Database[myEntry.hash()] = &myEntry;
    }
    myOpen("./FLOW", &file);
    while (getline(file, myLine)) {
        addToOutput(myLine);//this checks our hashmap for out function and construcs an output accordingly.
    }
    
    return 0;
    // now we loop back and get the next line in 'str'
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

void addToOutput(string myLine) {
    IP_Address myIP = IP_Address(myLine);//check hashmap and generate output accordingly
    IP_Address answer;
    unsigned long myIP_Key = myIP.key();
    NAT_Entry* myEntry;
    myEntry = NAT_Database[myIP.hash()];
    while (myEntry != NULL) {
        if (myEntry->key() == myIP_Key) {
            answer = myEntry->getTo();
        }
        myEntry = myEntry->getNext();
    }
    
    if (myEntry == NULL) {
        cout << "No nat match for " << myIP.to_string() << endl;
    } else {
        cout << myIP.to_string() << " -> " << answer.to_string() << endl;
    }
}
/**************************/
