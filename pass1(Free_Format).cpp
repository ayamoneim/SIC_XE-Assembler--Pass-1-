#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <set>

long STARTLOCATION = 0;

static const size_t npos = -1;

map<string,int>standardOperations;
vector<pair<string,long> >sympolTable;

std::set<string> usedLabels;
string DEFAULT ="-";
string byte = "";
int mem = 1;
map<string,long> symTab;
int MAXLABELSIZE = 8;
int MAXOPERATIONSIZE = 8;
int MAXOPERANDSIZE = 20;

#include <iomanip>
std::string int_to_hex( int i )
{
    std::stringstream stream;
    stream <<  std::hex << i;
    string ans = "";
    for(int i = 0; i<6-stream.str().length(); i++)
    {
        ans+="0";
    }
    ans+=stream.str();
    transform(ans.begin(), ans.end(), ans.begin(), ::toupper);

    return ans;
}
string print(int lineNumber,long location ,string label ,string operation,string operands,string comment,string errorMsg)
{
    stringstream ss ;
    string temp = "";
    ss<<(lineNumber+1);
    int counter = 0;
    int n = lineNumber+1;
    while(n!=0)
    {
        counter++;
        n/=10;

    }

    for(int j = 0; j<3-counter; j++)
        ss<<" ";
    ss<<"   ";
    ss<<int_to_hex(location);
    ss<<"   ";
    ss<< label;
    for(int j = 0; j<MAXLABELSIZE-label.length(); j++)
        ss<<" ";
    ss<<"   ";
    ss<< operation;
    for(int j = 0; j<MAXOPERATIONSIZE-operation.length(); j++)
        ss<<" ";
    ss<<"   ";
    ss<< operands;
    for(int j = 0; j<MAXOPERANDSIZE-operands.length(); j++)
        ss<<" ";
    ss<<"   ";
    ss<<comment<<'\n';
    ss<<"   "<<errorMsg<<'\n';
    temp = operation;
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    if(label!="/" && temp!="start")
    {
        temp = label;
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        sympolTable.push_back(make_pair(temp,location));

        symTab[temp] = location;
    }

    return ss.str();
}
bool canBeLabel(string lab)
{
    if(!isalpha(lab.at(0)))
    {
        return false;
    }

    if(lab.find(',')!=npos || lab.find('*')!=npos ||lab.find(' ')!=npos || lab.find('\t')!=npos || lab.find('\'')!=npos )
    {
        return false;
    }
    if(standardOperations.find(lab)!=standardOperations.end())
    {
        return false;
    }
    return true;
}
string findOperation(vector<string>token)
{
    string op = "/";
    string errorMsg = "";
    bool f = false;
    for(int i = 0; i<token.size(); i++)
    {
        string temp = token[i];
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        if(standardOperations.find(temp)!=standardOperations.end() && op=="/")
        {
            if(i>0 && token[i-1]=="+")
            {
                op = "+"+temp;
                if(standardOperations.find(op)!=standardOperations.end())
                {
                    op = "+"+token[i];
                }
                else
                {
                    op = token[i];
                }
            }
            else
            {
                op = token[i];
            }
        }
        else if(standardOperations.find(temp)!=standardOperations.end() && op!="/")
        {
            op = "INVALID?";
            f = true;
        }

    }
    if(op=="/")
    {
        op = "INVALID?";
    }
    if(f)
    {
        op = "/";
    }
    if(op.length()>MAXOPERATIONSIZE)
        MAXOPERATIONSIZE = op.length();
    return op;
}
/*
HANDLES LABEL REPETITION AND SPECIAL CHARACTERS
*/
bool validLabel(string lbl,int lineNumber)
{
    string temp = lbl;
    if(lbl.find(' ')!=npos || lbl.find('\t')!=npos || lbl.find('*')!=npos ||lbl.find(',')!=npos ||lbl.find('\'')!=npos)
    {
        return false;
    }
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    for (std::set<string>::iterator it=usedLabels.begin(); it!=usedLabels.end(); ++it)
    {
        string temp1 = *it;
        transform(temp1.begin(), temp1.end(), temp1.begin(), ::tolower);
        if(temp==temp1)
        {
            return false;
        }
    }
    if(standardOperations.find(temp)!=standardOperations.end())
    {
        return false;
    }
    return true;
}

string getLabel(vector<string>token,string op,int lineNumber)
{
    string lbl = "/";
    bool f = false;
    for(int i = 0; i<token.size(); i++)
    {
        if(token[i]==op || (i>0 && (token[i-1]+token[i])==op))
        {
            if(i>1 || isdigit(token[0].at(0)))
            {
                lbl = "INVALID?";
                f = true;
            }
            else if(i==0)
            {
                f = true;
            }
            else
            {
                lbl = token[0];
            }
            break;
        }
    }
    if(!f)
    {

        if(!validLabel(lbl,lineNumber) )
        {
            f = true;
        }
    }
    if(!f && lbl!="/")
    {
        usedLabels.insert(lbl);
    }

    string opL = op;
    transform(opL.begin(), opL.end(), opL.begin(), ::tolower);
    if(lbl=="/")
    {
        if(opL=="resb")
        {
            mem = 0;

            lbl = "INVALID?";
        }
        else if(opL=="resw")
        {
            mem = 0;
            lbl = "INVALID?";
        }


    }

    if(lbl.length()>MAXLABELSIZE)
        MAXLABELSIZE = lbl.length();
    return lbl;
}

bool isNumeric(int s,int t,string ss)
{
    for(int i = s; i<t; i++)
    {
        if(!isdigit(ss.at(i)))
            return false;
    }
    return true;
}
bool wordLiteral(string ss)
{
    if(ss.at(0)=='=' && ss.at(1)=='w' && ss.at(2)=='\''&& ss.at(ss.length()-1)=='\'')
    {
        if(ss.at(3)=='-' && isNumeric(4,ss.length()-5,ss))
        {
            return true;
        }
        else if(ss.at(3)!='-'  && isNumeric(3,ss.length()-4,ss))
        {
            return true;
        }
    }
    return false;
}
bool validOperands(string oper,string op,int lineNumber)
{

    string opL = op;
    transform(opL.begin(), opL.end(), opL.begin(), ::tolower);

    string operL = oper;
    transform(operL.begin(), operL.end(), operL.begin(), ::tolower);
    if(!(opL=="rsub" || opL=="fix" || opL=="float" || opL=="fix" || opL=="sio" || opL=="tio" || opL=="end") && oper.length()==0)
        return false;
    if(opL=="addr" || opL=="compr" ||opL == "divr" ||opL=="mulr" || opL=="rmo" || opL=="subr" || opL=="rmo")
    {

        if(operL.length()==3 &&(operL.at(0)=='x' || operL.at(0)=='a' || operL.at(0)=='l'||operL.at(0)=='b'||operL.at(0)=='s' ||operL.at(0)=='t'||operL.at(0)=='f')&&(operL.at(1)==',')&&((operL.at(2)=='x' || operL.at(2)=='a' || operL.at(2)=='l'||operL.at(2)=='b'||operL.at(2)=='s' ||operL.at(2)=='t'||operL.at(2)=='f')))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(opL=="fix" || opL=="float" || opL=="fix" || opL=="sio" || opL=="tio" || opL=="end" || opL=="rsub" || opL=="nobase")
    {

        return true;
    }
    else if(opL=="start")
    {
        for(int i = 0; i<operL.length(); i++)
        {
            if(isxdigit(operL.at(i)))
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        stringstream ss ;
        ss<<operL;
        ss>>std::hex>>STARTLOCATION;
        return true;
    }
    else if(opL=="clear" || opL=="tixr")
    {
        if(operL.length()==1)
        {
            if((operL.at(0)=='x' || operL.at(0)=='a' || operL.at(0)=='l'||operL.at(0)=='b'||operL.at(0)=='s' ||operL.at(0)=='t'||operL.at(0)=='f'))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else if(opL=="shiftr" || opL=="shiftl" || opL=="+shiftr" || opL=="+shiftl")
    {
        int commaIndex = operL.find(',');
        if(commaIndex<0)
        {
            return false;
        }
        string oper1 = operL.substr(0,commaIndex);
        string oper2 = operL.substr(commaIndex,operL.length()-commaIndex);
        if(oper1.length()==1 && (oper1.at(0)=='x' || oper1.at(0)=='a' || oper1.at(0)=='l'||oper1.at(0)=='b'||oper1.at(0)=='s' ||oper1.at(0)=='t'||oper1.at(0)=='f'))
        {
            for(int i = 0; i<oper2.length(); i++)
            {
                if(isdigit(oper2.at(i)))
                {
                    continue;
                }
                else
                {
                    return false;
                }

            }
            return true;
        }
        else
        {
            return false;
        }

    }
    else
    {
        int addressingMode = 0;
        if(operL.length()>1)
        {
            if(operL.at(0)=='@')
            {
                addressingMode = 1;
            }
            else if(operL.at(0)=='#')
            {
                addressingMode = 2;
            }
        }
        int commaIndex = operL.find(',');
        if(commaIndex==0)
            return false;
        if(commaIndex>0 && addressingMode!=0)
        {
            return false;
        }
        else if(commaIndex>0)
        {

            if(opL=="resb" || opL=="resw" )
            {
                return false;
            }

        }
        if(addressingMode!=0)
            operL = operL.substr(1,operL.length()-1);
        if(opL=="resb" || opL=="resw" || opL=="word")
        {
            if(operL.length()==0)
                return false;
            if(opL=="word")
            {
                //label
                bool f = canBeLabel(operL);
                if(f)
                {
                    return true;
                }
                //number
                if(operL.length()>=1 && ( operL.at(0)=='-' && isNumeric(1,operL.length(),operL))|| isNumeric(0,operL.length(),operL))
                {
                    return true;
                }
                //number ,number , number
                //number ,number , number
                string temp = operL;
                int i = 0;

                while(temp.length()!=0)
                {
                    if(temp.find(',')==npos)
                    {
                        string num = temp;
                        if(num.length()>=1 && ( num.at(0)=='-' && isNumeric(1,num.length(),num))|| isNumeric(0,num.length(),num))
                        {

                        }
                        else
                        {
                            return false;

                        }
                        temp = "";
                    }
                    else
                    {
                        int endIndex = temp.find(',');
                        string num = temp.substr(0,endIndex);
                        if(num.length()>=1 &&  ( num.at(0)=='-' && isNumeric(1,num.length(),num))|| isNumeric(0,num.length(),num))
                        {
                        }
                        else
                        {
                            return false;
                        }
                        temp = temp.substr(endIndex+1,temp.length()-endIndex-1);
                    }
                    i++;

                }

                if(temp.length()==0)
                {
                    mem = i;
                    return true;
                }
                return false;
            }
            else if(opL=="resb" || opL=="resw")
            {
                if(isNumeric(0,operL.length(),operL) && mem!=0)
                {
                    mem = atoi(operL.c_str());
                    return true;
                }
                return false;
            }

        }
        else if(opL=="byte")
        {

            if(operL.length()>=3 && (operL.at(0)=='c'|| operL.at(0)=='x')&& addressingMode==0)
            {
                if(operL.at(1)=='\'' && operL.at(operL.length()-1)=='\'')
                {
                    if(operL.at(0)=='x')
                    {
                        for(int i = 2; i<operL.length()-1; i++)
                        {
                            if(isxdigit(operL.at(i)))
                            {
                                continue;
                            }
                            else
                            {
                                return false;
                            }
                        }
                        if((operL.length()-3)%2!=0)
                            return false;
                        return true;
                    }
                    else
                    {
                        mem = byte.length()-3;
                        return true;
                    }
                }
            }
            return false;
        }
        else if(opL=="add" || opL=="+add" || opL=="sub" || opL=="+sub"|| opL=="div" || opL=="+div" || opL=="mul"||opL=="+mul")
        {
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }
            //number
            if(operL.length()>=1 && ( operL.at(0)=='-' && isNumeric(1,operL.length(),operL))|| isNumeric(0,operL.length(),operL))
            {
                return true;
            }

            //word literal
            if(addressingMode!=0)
            {
                return false;
            }
            else
            {
                if(wordLiteral(operL))
                {
                    return true;
                }

            }
            //label,x
            if(commaIndex>0 && addressingMode==0)
            {

                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        else if(opL=="j" || opL=="+j")
        {
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }
            //*
            if(operL.length()==1 && operL.at(0)=='*' && addressingMode==0)
            {
                return true;
            }
            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        else if(opL=="jeq" || opL=="+jeq" || opL=="jlt" || opL=="+jlt" || opL=="+jgt" || opL=="jgt" )
        {
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }
            //number
            if(isNumeric(0,operL.length(),operL))
            {
                return true;
            }
            if( operL.length()>=1 && operL.at(0)=='-' && isNumeric(1,operL.length(),operL) && addressingMode!=0)
            {
                return false;
            }
            // * -ve number

            if(operL.length()>=2 && operL.at(0)=='*')
            {
                if(operL.at(1)=='-' && isNumeric(2,operL.length(),operL))
                    return true;
                if(operL.at(1)=='+' && isNumeric(2,operL.length(),operL))
                    return true;
                if(isNumeric(1,operL.length(),operL))
                    return false;
            }

            //word literal
            if(addressingMode!=0)
            {
                return false;
            }
            else
            {
                if(wordLiteral(operL))
                {
                    return true;
                }

            }
            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        else if(opL=="jsub" || opL=="+jsub" )
        {
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }
            //+ve number
            if(isNumeric(0,operL.length(),operL))
            {
                return true;
            }
            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }


            return false;
        }
        else if(opL=="ldch" ||opL=="+ldch" || opL=="stch" ||opL=="+stch" ||opL=="stx" ||opL=="+stx"|| opL=="ldx" || opL=="+ldx")
        {

            if(operL.length()>=1 && operL.at(0)=='-')
            {
                return false;
            }
            else
            {
                //+ve number
                if(isNumeric(0,operL.length(),operL))
                    return true;

                //label
                bool f = canBeLabel(operL);
                if(f)
                {
                    return true;
                }
                //label,x
                if(commaIndex>0 && addressingMode==0)
                {
                    string oper1 = operL.substr(0,commaIndex);
                    string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                    if(oper2=="x")
                    {
                        bool f = canBeLabel(oper1);
                        if(f)
                        {
                            return true;
                        }
                    }
                }

            }
            if(opL=="ldch" || opL=="+ldch")
            {
                //literal character
                if(operL.length()==5 &&  operL.at(0)=='=' && operL.at(1)=='c' && operL.at(2)=='\'' && operL.at(4)=='\'')
                    return true;

                //word literal
                if(addressingMode!=0)
                {
                    return false;
                }
                else
                {
                    if(wordLiteral(operL))
                    {
                        return true;
                    }

                }

                return false;
            }
            if(opL=="ldx" || opL=="+ldx")
            {

                //word literal
                if(addressingMode!=0)
                {
                    return false;
                }
                else
                {
                    if(wordLiteral(operL))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }

            }
            return false;
        }
        else if(opL=="lda" || opL=="+lda" || opL=="sta" ||opL=="+sta")
        {
            if(opL=="lda" || opL=="+lda")
            {
                //literal
                if(operL.length()>=3 &&  operL.at(0)=='=' && (operL.at(1)=='c' || operL.at(1)=='x')&& addressingMode==0)
                {
                    if(operL.at(2)=='\'' && operL.at(operL.length()-1)=='\'')
                    {
                        if(operL.at(1)=='x')
                        {
                            for(int i = 3; i<operL.length()-1; i++)
                            {
                                if(isxdigit(operL.at(i)))
                                {
                                    continue;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            if((operL.length()-4)%2==0)
                                return true;
                            return false;
                        }
                        else
                        {
                            return true;
                        }
                    }
                    return false;
                }
                //label
                bool f = canBeLabel(operL);
                if(f)
                {
                    return true;
                }
                //word literal
                if(wordLiteral(operL) && addressingMode==0)
                {
                    return true;
                }
                //number
                if(isNumeric(0,operL.length(),operL))
                    return true;

                //label,x
                if(commaIndex>0 && addressingMode==0)
                {
                    string oper1 = operL.substr(0,commaIndex);
                    string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                    if(oper2=="x")
                    {
                        bool f = canBeLabel(oper1);
                        if(f)
                        {
                            return true;
                        }
                    }
                }
                return false;
            }
            else if(opL=="sta" ||opL=="+sta")
            {
                //+ve number
                if(isNumeric(0,operL.length(),operL))
                    return true;
                //label
                bool f = canBeLabel(operL);
                if(f)
                {
                    return true;
                }

                //label,x
                if(commaIndex>0 && addressingMode==0)
                {
                    string oper1 = operL.substr(0,commaIndex);
                    string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                    if(oper2=="x")
                    {
                        bool f = canBeLabel(oper1);
                        if(f)
                        {
                            return true;
                        }
                    }
                }

            }
            return false;
        }
        else if(opL=="comp" || opL=="+comp")
        {
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return true;
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return true;
            }

            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }


        }
        else if(opL=="td" || opL=="+td" || opL=="rd" || opL=="+rd")
        {
            // hex literal
            if(operL.length()>=3 &&  operL.at(0)=='=' && operL.at(2)=='\'' && operL.at(1)=='x' && operL.at(operL.length()-1)=='\'')
            {
                for(int i = 3; i<operL.length()-1; i++)
                {
                    if(isxdigit(operL.at(i)))
                    {
                        continue;
                    }
                    else
                    {
                        return false;
                    }
                }
                return true;
            }
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return true;
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return true;
            }

            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }

        }
        else if(opL=="+ldb" || opL=="+lds" || opL=="+ldt" || opL=="+ldl" || opL=="ldb" || opL=="lds" || opL=="ldt" || opL=="ldl")
        {
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return true;
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return true;
            }

            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        else if((opL=="+stb" || opL=="+sts" || opL=="+stt" || opL=="+stl" || opL=="stb" || opL=="sts" || opL=="stt" || opL=="stl"))
        {
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return true;
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }

            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        else if(opL=="wd" && opL=="+wd")
        {
            //literal character
            if(operL.length()==5 &&  operL.at(0)=='=' && operL.at(1)=='c' && operL.at(2)=='\'' && operL.at(4)=='\'')
                return true;
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return true;
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }

            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return true;
            }
            return false;
        }
        else if(opL=="+tix" && opL=="tix")
        {
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return true;
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return true;
            }

            //label,x
            if(commaIndex>0 && addressingMode==0)
            {
                string oper1 = operL.substr(0,commaIndex);
                string oper2 = operL.substr(commaIndex+1,operL.length()-commaIndex-1);
                if(oper2=="x")
                {
                    bool f = canBeLabel(oper1);
                    if(f)
                    {
                        return true;
                    }
                }
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return true;
            }
            return false;
        }

    }
    return true;
}
string getOperands(vector<string>token,string op,int lineNumber)
{
    string temp = "";

    for(int i = 0; i<token.size(); i++)
    {
        if(token[i]==op)
        {
            for(int j = i+1; j<token.size(); j++)
            {
                temp+=token[j];
            }
            break;
        }
    }
    if(temp.length()>MAXOPERANDSIZE )
        MAXOPERANDSIZE = temp.length();
    if(byte.length()>MAXOPERANDSIZE)
        MAXOPERANDSIZE = byte.length();

    return temp;
}
inline std::string trim(std::string& str)
{
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces
    return str;
}

void read_Parse_File()
{
    ifstream file("input3.txt");
    if (file.is_open())
    {
        ofstream myfile;
        bool haveEnd = false;
        myfile.open("output.txt");
        myfile<<"Line"<<"     "<<"LOCATION"<<"       "<<"LABEL"<<"      "<<"OPERATION"<<"    "<<"OPERANDS"<<"               "<<"COMMENTS"<<'\n';
        cout<<"Line"<<"     "<<"LOCATION"<<"       "<<"LABEL"<<"      "<<"OPERATION"<<"    "<<"OPERANDS"<<"               "<<"COMMENTS"<<'\n';
        myfile<<"-----------------------------------------------------------------------------------------------------------------"<<'\n';
        cout<<"-----------------------------------------------------------------------------------------------------------------"<<'\n';

        long location = STARTLOCATION;
        string temp = "";
        int lineNumber = 0;
        bool started = false;
        while(!file.eof() && myfile.is_open())
        {
            string line;
            getline(file,line);
            string comment = "";
            string operand = "";
            string operation = "";
            string errors = "";
            string label = "";
            //First :Extract comment from line


            mem = 1;


            int commentStartPosition = line.find('.');
            if(commentStartPosition>=0)
            {
                comment+=line.substr(commentStartPosition,line.length()-commentStartPosition);
                line = line.substr(0,commentStartPosition);
            }
            if(commentStartPosition==-1)
            {
                comment+="/";
            }
            int quoteStartPosition = line.find('\'');
            if(quoteStartPosition>0)
            {
                byte='c'+line.substr(quoteStartPosition,line.length()-quoteStartPosition);
                trim(byte);
            }
            std::istringstream iss(line);
            vector<string>token;
            copy(istream_iterator<string>(iss),
                 istream_iterator<string>(),
                 back_inserter(token));
            if(token.size()!=0 || commentStartPosition>=0)
            {
                if(commentStartPosition>0 || commentStartPosition==-1)
                {
                    operation= findOperation(token);
                    if(operation=="INVALID?")
                    {
                        errors+="ERROR IN OPERATION FIELD\n";
                        mem = 0;
                        //            operation = "/";
                    }
                    label = getLabel(token,operation,lineNumber);
                    if(label=="INVALID?")
                    {
                        errors+="ERROR IN LABEL FIELD\n";
                        //          label = "/";
                    }
                    operand = getOperands(token,operation,lineNumber);
                    temp = operation;
                    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
                    if(!validOperands(operand,operation,lineNumber))
                    {
                        errors+="ERROR IN OPERANDS FIELD\n";
                        //        operand = "/";
                    }
                    else
                    {
                        if(temp=="byte" && byte!="")
                        {
                            operand = byte;
                        }
                    }

                    stringstream out ;

                    if(!started){
                            location = STARTLOCATION;
                    out<<print(lineNumber,location,label,operation,operand,comment,errors);
                    cout<<out.str();
                    myfile<<out.str();
                    }else{
                        out<<print(lineNumber,location,label,operation,operand,comment,errors);
                        cout<<out.str();
                        myfile<<out.str();
                    if(temp=="start")
                    {

                        if(started){
                            myfile<<"Misplaced or Duplicate Start statement\n";
                            cout<<"Misplaced or Duplicate Start statement\n";
                        }

                    }
                    }
                    started = true;
                }
                else if(commentStartPosition==0)
                {
                    stringstream out ;
                    if(!started)
                        location = STARTLOCATION;
                    out<<print(lineNumber,location,"/","/","/",comment,"/");
                    cout<<out.str();
                    myfile<<out.str();


                }

                string temp = operation;
                transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
                if(temp!="/" && temp!="")
                    location+=standardOperations[temp]*mem;
                if(temp=="end")
                {
                    if(haveEnd)
                    {
                        myfile<<"Misplaced or Duplicate end statement\n";
                        cout<<"Misplaced or Duplicate end statement\n";
                    }
                    haveEnd = true;
                }

            }
            else
            {
                lineNumber--;
            }
            byte = "";
            lineNumber++;

        }
        if(!haveEnd)
        {
            myfile<<"Missing End Mnemonic\n";
            cout<<"Missing End Mnemonic\n";
        }
        myfile<<">>    e n d    o f   p a s s   1 \n>>   *****************************************************\n>>    s y m b o l     t a b l e   (values in decimal)\n        name         value     \n        -----------------------\n";
        cout<<">>    e n d    o f   p a s s   1 \n>>   *****************************************************\n>>    s y m b o l     t a b l e   (values in decimal)\n        name         value     \n        -----------------------\n";

        for(int j = 0; j<sympolTable.size(); j++)
        {
            myfile<<"        ";
            cout<<"        ";
            string temp = sympolTable[j].first;
            myfile<<temp;
            cout<<temp;
            for(int j = 0; j<MAXLABELSIZE-temp.length(); j++)
            {
                myfile<<" ";
                cout<<" ";
            }
            myfile<<"        "<<sympolTable[j].second<<'\n';
            cout<<"        "<<sympolTable[j].second<<'\n';

        }
        myfile.close();
    }
    file.close();
}
void setOperationsVector()
{
    ifstream file("formats.txt");
    vector<int>format;
    if (file.is_open())
    {
        while(!file.eof())
        {
            string line;
            getline(file,line);
            format.push_back(atoi(line.c_str()));
        }
    }
    file.close();
    file.open("operations.txt");
    if (file.is_open())
    {
        int i = 0;
        while(!file.eof())
        {
            string line;
            getline(file,line);
            standardOperations[line] = format[i];

            i++;
        }
    }
    file.close();
}

int main()
{
    string entered ="";
    while(entered!="pass1 <main>")
    {
        getline(std::cin,entered);
    }
    setOperationsVector();
    read_Parse_File();
    return 0;
}
