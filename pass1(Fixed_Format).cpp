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

vector<pair<string,long> >sympolTable;
map<string,int>standardOperations;
map<string,long> symTab;
std::set<string> usedLabels;
string DEFAULT ="-";
string byte = "";
int mem = 1;

int MAXLABELSIZE = 8;
int MAXOPERATIONSIZE = 6;
int MAXOPERANDSIZE = 18;
static const size_t npos = -1;
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

    if(lab.length()>8)
    {

        return false;
    }
    if(standardOperations.find(lab)!=standardOperations.end())
    {
        return false;
    }
    return true;
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
std::string validLabel(string lbl)
{
    string temp = lbl;

    if(lbl.find(' ')!=npos || lbl.find('\t')!=npos || lbl.find('*')!=npos ||lbl.find(',')!=npos ||lbl.find('\'')!=npos)
    {
        return false;
    }
    if(lbl.empty())
        return "";
    string error = "";
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    for (std::set<string>::iterator it=usedLabels.begin(); it!=usedLabels.end(); ++it)
    {
        string temp1 = *it;
        transform(temp1.begin(), temp1.end(), temp1.begin(), ::tolower);
        if(temp==temp1)
        {
            error+="Error:Label is Already Used\n";
            return error;
        }
    }
    if(standardOperations.find(temp)!=standardOperations.end())
    {
        error+="Error:Using the mnemonics as labels is not allowed.\n";
        return error;
    }
    return error;
}

std::string validOperands(string oper,string op ,string lbl)
{
    if(op=="")
        return "";
    string opL = op;
    transform(opL.begin(), opL.end(), opL.begin(), ::tolower);
    string errorMsg = "";

    string operL = oper;
    transform(operL.begin(), operL.end(), operL.begin(), ::tolower);
    if(!(opL=="rsub" || opL=="fix" || opL=="float" || opL=="fix" || opL=="sio" || opL=="tio" || opL=="end") && oper.length()==0)
        return "Operands Required";
    if(opL=="addr" || opL=="compr" ||opL == "divr" ||opL=="mulr" || opL=="rmo" || opL=="subr" || opL=="rmo")
    {

        if(operL.length()==3 &&(operL.at(0)=='x' || operL.at(0)=='a' || operL.at(0)=='l'||operL.at(0)=='b'||operL.at(0)=='s' ||operL.at(0)=='t'||operL.at(0)=='f')&&(operL.at(1)==',')&&((operL.at(2)=='x' || operL.at(2)=='a' || operL.at(2)=='l'||operL.at(2)=='b'||operL.at(2)=='s' ||operL.at(2)=='t'||operL.at(2)=='f')))
        {
            return "";
        }
        else
        {
            return "[Operands Are Not Compatible With The Operation]";
        }
    }
    else if(opL=="fix" || opL=="float" || opL=="fix" || opL=="sio" || opL=="tio" || opL=="end" || opL=="rsub" || opL=="nobase")
    {

        return "";
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
                return "[Operand Contains Non Hexadecimal Character]";

            }
        }
        stringstream ss ;
        ss<<operL;
        ss>>std::hex>>STARTLOCATION;
        return "";
    }
    else if(opL=="clear" || opL=="tixr")
    {
        if(operL.length()==1)
        {
            if(operL.length()>=1 && (operL.at(0)=='x' || operL.at(0)=='a' || operL.at(0)=='l'||operL.at(0)=='b'||operL.at(0)=='s' ||operL.at(0)=='t'||operL.at(0)=='f'))
            {
                return "";
            }
            else
            {
                return "[Operand Should be a Register]";
            }
        }
        else
        {
            return "Incorrect Operands";
        }
    }
    else if(opL=="shiftr" || opL=="shiftl" || opL=="+shiftr" || opL=="+shiftl")
    {
        int commaIndex = operL.find(',');
        if(commaIndex==npos)
        {
            return "[Operands are (r,n)]";
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
                    return "[Operands are (r,n)]";

                }

            }
            return "";
        }
        else
        {
            return "Incorrect Operands";
        }

    }
    else
    {
        int addressingMode = 0;
        if(operL.length()>1){
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
            return "Incorrect Operands";
        if(commaIndex>0 && addressingMode!=0)
        {
            return "[Op m,x]";
        }
        if((opL=="resb" || opL=="resw" ) && lbl.empty())
        {
            mem = 0;
            return "Operation Requires Label";

        }
        else if(commaIndex>0)
        {

            if(opL=="resb" || opL=="resw" )
            {
                mem = 0;
                return "Incorrect Operands";
            }

        }
        if(addressingMode!=0)
            operL = operL.substr(1,operL.length()-1);
        if(opL=="resb" || opL=="resw" || opL=="word")
        {
            if(operL.length()==0){
                mem = 0;
                return "Incorrect Operands";
            }
            if(opL=="word")
            {


                //label
                bool f = canBeLabel(operL);
                if(f)
                {
                    return "";
                }
                //number
                if(operL.length()>=1 && ( operL.at(0)=='-' && isNumeric(1,operL.length(),operL))|| isNumeric(0,operL.length(),operL))
                {
                    return "";
                }
                //number ,number , number
                string temp = operL;
                int i = 0;

                while(temp.length()!=0)
                {
                    if(temp.find(',')==npos)
                    {
                        string num = temp;
                        if(num.length() >=1 && ( num.at(0)=='-' && isNumeric(1,num.length(),num))|| isNumeric(0,num.length(),num))
                        {

                        }
                        else
                        {
                            return "Incorrect Operands";

                        }
                        temp = "";
                    }
                    else
                    {
                        int endIndex = temp.find(',');
                        string num = temp.substr(0,endIndex);
                        if(num.length() >=1 &&( num.at(0)=='-' && isNumeric(1,num.length(),num))|| isNumeric(0,num.length(),num))
                        {
                        }
                        else
                        {
                            return "Incorrect Operands";
                        }
                        temp = temp.substr(endIndex+1,temp.length()-endIndex-1);
                    }
                    i++;

                }

                if(temp.length()==0)
                {
                    mem = i;
                    return "";
                }
                return "Incorrect Operands";
            }
            else if(opL=="resb" || opL=="resw")
            {
                if(isNumeric(0,operL.length(),operL))
                {
                    mem = atoi(operL.c_str());
                    return "";
                }
                mem = 0;
                return "Incorrect Operands";
            }
            return "[Illegal Operands]";


        }
        else if(opL=="byte")
        {
            //  cout<<(operL.at(operL.length()-1));


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
                                return "[Operand Contains Non Hexadecimal Character]";
                            }
                        }
                        if((operL.length()-3)%2!=0)
                            return "Not Hexa Literal";
                        return "";
                    }
                    else if(operL.length()>=3 && operL.at(0)=='c')
                    {
                        //  cout<<"in2\n\n";
                        mem = byte.length()-3;
                        return "";
                    }
                }
            }
            cout<<operL<<'\n';
            //cout<<"fuck"<<byte;
            return "Incorrect Operands";
        }
        else if(opL=="add" || opL=="+add" || opL=="sub" || opL=="+sub"|| opL=="div" || opL=="+div" || opL=="mul"||opL=="+mul")
        {
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
            }
            //number
            if(operL.length()>=1 &&( operL.at(0)=='-' && isNumeric(1,operL.length(),operL))|| isNumeric(0,operL.length(),operL))
            {
                return "";
            }

            //word literal
            if(addressingMode!=0)
            {
                return "Incorrect Operands";
            }
            else
            {
                if(wordLiteral(operL))
                {
                    return "";
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
                        return "";
                    }
                }

            }
            return "Incorrect Operands";
        }
        else if(opL=="j" || opL=="+j")
        {
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
            }
            //*
            if(operL.length()==1 && operL.at(0)=='*' && addressingMode==0)
            {
                return "";
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
                        return "";
                    }
                }
            }
            return "Incorrect Operands";
        }
        else if(opL=="jeq" || opL=="+jeq" || opL=="jlt" || opL=="+jlt" || opL=="+jgt" || opL=="jgt" )
        {
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
            }
            //number
            if(isNumeric(0,operL.length(),operL))
            {
                return "";
            }
            if(operL.length()>=1 &&  operL.at(0)=='-' && isNumeric(1,operL.length(),operL) && addressingMode!=0)
            {
                return "Incorrect Operands";
            }
            // * -ve number

            if(operL.length()>=2 && operL.at(0)=='*')
            {
                if(operL.at(1)=='-' && isNumeric(2,operL.length(),operL))
                    return "";
                if(operL.at(1)=='+' && isNumeric(2,operL.length(),operL))
                    return "";
                if(isNumeric(1,operL.length(),operL))
                    return "Incorrect Operands";
            }

            //word literal
            if(addressingMode!=0)
            {
                return "Incorrect Operands";
            }
            else
            {
                if(wordLiteral(operL))
                {
                    return "";
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
                        return "";
                    }
                }
            }
            return "Incorrect Operands";
        }
        else if(opL=="jsub" || opL=="+jsub" )
        {
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
            }
            //+ve number
            if(isNumeric(0,operL.length(),operL))
            {
                return "";
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
                        return "";
                    }
                }
            }


            return "Incorrect Operands";
        }
        else if(opL=="ldch" ||opL=="+ldch" || opL=="stch" ||opL=="+stch" ||opL=="stx" ||opL=="+stx"|| opL=="ldx" || opL=="+ldx")
        {

            if(operL.length()>=1 && operL.at(0)=='-')
            {
                return "Incorrect Operands";
            }
            else
            {
                //+ve number
                if(isNumeric(0,operL.length(),operL))
                    return "";

                //label
                bool f = canBeLabel(operL);
                if(f)
                {
                    return "";
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
                            return "";
                        }
                    }
                }

            }
            if(opL=="ldch" || opL=="+ldch")
            {
                //literal character
                if(operL.length()==5 &&  operL.at(0)=='=' && operL.at(1)=='c' && operL.at(2)=='\'' && operL.at(4)=='\'')
                    return "";

                //word literal
                if(addressingMode!=0)
                {
                    return "Incorrect Operands";
                }
                else
                {
                    if(wordLiteral(operL))
                    {
                        return "";
                    }

                }
                return "Incorrect Operands";


            }
            if(opL=="ldx" || opL=="+ldx")
            {

                //word literal
                if(addressingMode!=0)
                {
                    return "Incorrect Operands";
                }
                else
                {
                    if(wordLiteral(operL))
                    {
                        return "";
                    }

                }

            }
            return "Incorrect Operands";
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
                                    return "[Operand Contains Non Hexadecimal Character]";
                                }
                            }
                            return "";
                        }
                        else
                        {
                            return "";
                        }
                    }
                    return "Incorrect Operands";
                }
                //label
                bool f = canBeLabel(operL);
                if(f)
                {

                    return "";
                }
                if(wordLiteral(operL) && addressingMode==0)
                {
                    return "";
                }
                if(isNumeric(0,operL.length(),operL))
                    return "";

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
                            return "";
                        }
                    }
                }
                return "Incorrect Operands";
            }
            else if(opL=="sta" ||opL=="+sta")
            {
                //+ve number
                if(isNumeric(0,operL.length(),operL))
                    return "";
                //label
                bool f = canBeLabel(operL);
                if(f)
                {
                    return "";
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
                            return "";
                        }
                    }
                }


            }
            return "Incorrect Operands";
        }
        else if(opL=="comp" || opL=="+comp")
        {
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return "";
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return "";
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
                        return "";
                    }
                }
            }
            return "[Illegal Operands]";

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
                        return "[Operand Contains Non Hexadecimal Character]";

                    }
                }
                return "";
            }
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return "";
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return "";
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
                        return "";
                    }
                }
            }
            return "[Illegal Operands]";

        }
        else if(opL=="+ldb" || opL=="+lds" || opL=="+ldt" || opL=="+ldl" || opL=="ldb" || opL=="lds" || opL=="ldt" || opL=="ldl")
        {
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return "";
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return "";
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
                        return "";
                    }
                }
            }
            return "Incorrect Operands";
        }
        else if((opL=="+stb" || opL=="+sts" || opL=="+stt" || opL=="+stl" || opL=="stb" || opL=="sts" || opL=="stt" || opL=="stl"))
        {
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return "";
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
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
                        return "";
                    }
                }
            }
            return "Incorrect Operands";
        }
        else if(opL=="wd" && opL=="+wd")
        {
            //literal character
            if(operL.length()==5 &&  operL.at(0)=='=' && operL.at(1)=='c' && operL.at(2)=='\'' && operL.at(4)=='\'')
                return "";
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return "";
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
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
                        return "";
                    }
                }
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return "";
            }
            return "Incorrect Operands";
        }
        else if(opL=="+tix" && opL=="tix")
        {
            //+ve number
            if(isNumeric(0,operL.length(),operL))
                return "";
            //label
            bool f = canBeLabel(operL);
            if(f)
            {
                return "";
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
                        return "";
                    }
                }
            }
            //word literal
            if(wordLiteral(operL) && addressingMode==0)
            {
                return "";
            }
            return "Incorrect Operands";
        }

    }
    return "";
}
#include <iomanip>
std::string int_to_hex( long i )
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
    ss<<errorMsg<<'\n';
    temp = operation;
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

    if(label!="" && label!="--" && temp!="start")
    {
        temp = label;
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        sympolTable.push_back(make_pair(temp,location));
        symTab[temp] = location;
    }

    return ss.str();
}
string validOperation(string operation)
{
    string temp = operation;
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    if(standardOperations.find(temp)==standardOperations.end())
    {
        return "Error : Invalid Mnemonic Sytax\n";
    }
    return "";
}
inline std::string trim(std::string& str)
{
    str.erase(0, str.find_first_not_of(' '));       //prefixing spaces
    str.erase(str.find_last_not_of(' ')+1);         //surfixing spaces
    return str;
}
void read_Parse_File()
{
    ifstream file("4.txt");
    if (file.is_open())
    {
        bool haveEnd = false;
        ofstream myfile;
        myfile.open("output.txt");
        myfile<<"Line"<<"     "<<"LOCATION"<<"       "<<"LABEL"<<"      "<<"OPERATION"<<"    "<<"OPERANDS"<<"             "<<"COMMENTS"<<'\n';
        cout<<"Line"<<"     "<<"LOCATION"<<"       "<<"LABEL"<<"      "<<"OPERATION"<<"    "<<"OPERANDS"<<"             "<<"COMMENTS"<<'\n';
        myfile<<"---------------------------------------------------------------------------------------------------------------"<<'\n';
        cout<<"---------------------------------------------------------------------------------------------------------------"<<'\n';
        long location = STARTLOCATION;
        bool started = false;
        int lineNumber = 0;
        vector<string>tempOperands;
        while(!file.eof())
        {
            string line;
            getline(file,line);
            string lbl ;
            string errorMsg ;
            string operand ;
            string operation ;
            string comment ;


            if(line.empty())
            {
                continue;
                lineNumber--;
            }

            mem = 1;


            int commentStartPosition = line.find('.');
            if(commentStartPosition>=0)
            {
                comment+=line.substr(commentStartPosition,line.length()-commentStartPosition);
                if(comment.length()>66-35 && commentStartPosition>0)
                {
                    errorMsg+="Too Long Comment\n";
                }
                line = line.substr(0,commentStartPosition);
            }
            if(commentStartPosition==npos)
            {
                comment+="";
            }
            int quoteStartPosition = line.find('\'');
            if(quoteStartPosition>0 &&( line.at(quoteStartPosition-1)=='c'||line.at(quoteStartPosition-1)=='C'))
            {
                byte='c'+line.substr(quoteStartPosition,line.length()-quoteStartPosition);
                trim(byte);


            }
            bool here = false;


            if(commentStartPosition>0 || commentStartPosition==npos)
            {
                int tempInt = -1;
                for(int i = 0; i<8 && line.length()>i; i++)
                {
                    if(line.at(i)=='\t')
                    {
                        here = true;
                        break;
                    }
                    if(i==0 && isdigit(line.at(i))){
                        errorMsg+="Error:Invalid Label Syntax[First Character Must be a Letter]\n";
                        lbl = "";
                        break;
                    }
                    if(line.at(i)!=' ')
                    {
                        lbl+=line.at(i);

                    }
                    else
                    {
                        tempInt = i;
                        break;
                    }
                }
                string temp = "";
                bool tempBool = false;
                if(tempInt>0)
                {
                    temp = line.substr(tempInt,8-lbl.length());
                    for(int i = 0; i<temp.length(); i++)
                    {
                        if(temp.at(i)!=' ')
                        {
                            if(!here)
                                errorMsg+="Error:Line Format Error [No whitespace]n";
                            tempBool = true;
                            here = true;

                            break;
                        }
                    }
                }
                if(!tempBool && lbl!="")
                {

                    temp = validLabel(lbl);
                    if(temp =="")
                    {
                        usedLabels.insert(lbl);


                    }
                    else
                    {
                        lbl = "";
                        errorMsg+=temp;
                        here = true;
                    }


                }

                if(line.length()>8 &&line.at(8)!=' ')
                {
                    if(!here)
                        errorMsg+="Error:Line Format Error \n";
                    here = true;

                }
                tempInt = -1;

                for(int i = 9; i<15 && line.length()>i; i++)
                {
                    if(line.at(i)=='\t')
                    {
                        here = true;
                        break;
                    }
                    if(line.at(i)==' ')
                    {

                        tempInt = i;
                        break;
                    }
                    operation+=line.at(i);
                }
                tempBool = false;
                if(tempInt>=0)
                {
                    temp = line.substr(tempInt,(15-9)-operation.length());
                    for(int i = 0; i<temp.length(); i++)
                    {
                        if(temp.at(i)!=' ')
                        {
                            if(!here)
                                errorMsg+="Error:Line Format Error [No whitespace]\n";
                            tempBool = true;
                            here = true;

                            break;
                        }
                    }
                }
                if(!tempBool && operation!="")
                {

                    temp = validOperation(operation);
                    if(temp!="")
                    {
                        operation = "";
                        errorMsg+=temp;
                    }

                }
                else if(operation=="")
                {
                    errorMsg+="Error:Empty Operation Field\n";
                }


                if((line.length()>15 && line.at(15)!=' ')||(line.length()>16 && line.at(16)!=' ') )
                {
                    if(!here)
                        errorMsg+="Error:Line Format Error \n";
                    here = true;

                }

                tempInt = -1;
                if(byte!="")
                {
                    operand = byte;
                }
                else
                {
                    for(int i = 17; i<35 && line.length()>i; i++)
                    {
                        if(line.at(i)=='\t')
                        {
                            here = true;
                            break;
                        }
                        if(line.at(i)==' ')
                        {
                            tempInt = i;
                            break;
                        }
                        operand+=line.at(i);
                    }

                    //Handle c'test tab and space '

                    tempBool = false;
                    if(tempInt>0)
                    {
                        temp = line.substr(tempInt,(35-17)-operand.length());
                        for(int i = 0; i<temp.length(); i++)
                        {
                            if(temp.at(i)!=' ')
                            {
                                if(!here)
                                    errorMsg+="Error:Line Format Error [No whitespace]\n";
                                tempBool = true;
                                here = true;
                                break;
                            }
                        }
                    }

                }
                                                                                    cout<<"here"<<'\n';

                if(!tempBool  )
                {

                    errorMsg+=validOperands(operand,operation,lbl);

                }

                for(int i = 35; i<66 && line.length()>i; i++)
                    comment+=line.at(i);
                if(line.length()>66)
                {
                    if(!here)
                        errorMsg+="Line Format Error -->Line Too Long\n";
                    here = true;
                }

                if(here)
                {
                    lbl = "--";
                    operand = "--";
                    operation = "--";
                    comment = "--";

                }
                temp = operation;
                transform(temp.begin(), temp.end(), temp.begin(), ::tolower);



            }

            byte = "";
            stringstream out ;
            string temp = operation;
            transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

            out<<print(lineNumber,location,lbl,operation,operand,comment,errorMsg);
            cout<<out.str();
            myfile<<out.str();
            if(temp=="start" && !started)
            {
                location = STARTLOCATION;
                            started = true;

            }
            else if(temp=="start")
            {
                myfile<<"Duplicate Or Misplaced start statement\n";
                cout<<"Duplicate Or Misplaced start statement\n";
            }
            if(temp!="" && temp!="/" && temp!="--" && !here)
            {
                location+=standardOperations[temp]*mem;
            }
            if(temp=="end")
            {

                if(haveEnd)
                {
                    myfile<<"Duplicate Or Misplaced end statement\n";
                    cout<<"Duplicate Or Misplaced end statement\n";

                }
                haveEnd = true;
            }


            lineNumber++;
        }


        if(!haveEnd)
        {
            myfile<<"Missing End statement\n";
            cout<<"Missing End statement\n";

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
