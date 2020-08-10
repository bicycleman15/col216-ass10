#include <bits/stdc++.h>
using namespace std;
#define forn(i, n)for(int i=0;i<int(n);i++)
#define sz(a) int(a.size())
template <class A, class B> ostream& operator << (ostream& out, const pair<A, B> &a){return out << "(" << a.first << ", " << a.second << ")";}
template<class A>ostream& operator<<(ostream& out,const vector<A>&v){out<<"[";forn(i,sz(v)){if(i)out<<", ";out<<v[i];}return out<<"]";}
#define watch(x) cout << #x << " : " << x << endl;

// HIT MISS
int X = 50;
int N = 10;

long long int binaryToInt(string s, int sign, int n = 32){
        long long int base = (long long int)pow(2,n);
//    watch(s);
        if(sign==1 && s[0]=='1')
            return stoll(s, nullptr, 2) - base;
        else
            return stoll(s, nullptr, 2);
}

string intToBinary(long long int n)
{
    if (n < 0)
        n = 4294967296 + n;
    string result = "";
    while(n > 0)
    {
        result = string(1, (char) (n%2 + 48)) + result;
        n = n/2;
    }
    while(result.length()<32)
        result = "0"+result;
    return result;
}

string signExtend(string str)
{
    if(str[0] == '0')
    {
        string s = "0000000000000000";
        s = s + str;
        return s;
    }
    else
    {
        string s = "1111111111111111";
        s = s + str;
        return s;
    }
}

const string zerosx32 = "00000000000000000000000000000000";

vector<string> instructionMemory;
vector<string> registers(32, zerosx32);
vector<string> dataMemory(10, zerosx32);
int clockCount = 0;

// pipeline registers
string PCreg[2];
vector<vector<string> > IF_ID(3,vector<string> (2, zerosx32)); // PC+1; Instruction; PC
vector<vector<string> > ID_EX(12,vector<string> (2, zerosx32)); // WBSig; MEMSig; EXSig; PC+1; DATA1; DATA2; Constant32; rt; rd; rs; jaddr; PC;
vector<vector<string> > EX_MEM(9,vector<string> (2, zerosx32));// WBSig; MEMSig; NewPC; ALUzero; ALUresult; WriteData; DestReg; jaddr; PC
vector<vector<string> > MEM_WB(5,vector<string> (2, zerosx32)); // WBSig; ReadData; ALUresult; DestReg; PC

string handle32(string s) {
    if(s == zerosx32) return "32x0";
    else return s;
}

void print_registers() {
    cout << "Registers are:" << endl;
    for(int i=0;i<32;i++) {
        cout << i << ": " << binaryToInt(registers[i],1, 32) << ", ";
    }
    cout << "-------------" << endl << endl;
}

void print_memory() {
    cout << "Memory is:" << endl;
    for(int i=0;i<10;i++) {
        cout << i << ": " << binaryToInt(dataMemory[i],1, 32) << ", ";
    }
    cout << "-------------" << endl << endl;
}

void print_IF_ID() {
    cout << "IF_ID is:" << endl;
    for(int i=0;i<3;i++) {
        cout << i << ": " << handle32(IF_ID[i][0]) << " " << handle32(IF_ID[i][1]) << endl;
    }
    cout << "-------------" << endl << endl;
}

void print_ID_EX() {
    cout << "ID_EX is:" << endl;
    for(int i=0;i<12;i++) {
        cout << i << ": " << handle32(ID_EX[i][0]) << " " << handle32(ID_EX[i][1]) << endl;
    }
    cout << "-------------" << endl << endl;
}

void print_EX_MEM() {
    cout << "EX_MEM is:" << endl;
    for(int i=0;i<9;i++) {
        cout << i << ": " << handle32(EX_MEM[i][0]) << " " << handle32(EX_MEM[i][1]) << endl;
    }
    cout << "-------------" << endl << endl;
}

void print_MEM_WB() {
    cout << "MEM_WB is:" << endl;
    for(int i=0;i<5;i++) {
        cout << i << ": " << handle32(MEM_WB[i][0]) << " " << handle32(MEM_WB[i][1]) << endl;
    }
    cout << "-------------" << endl << endl;
}

void CONTROL(string instruction)
{
    if( instruction == "000000") // no opcode
    {
        string RegWrite = "1";
        string MemtoReg = "0";
        string WBSig = RegWrite + MemtoReg;

        string Branch = "0";
        string MemRead = "0";
        string MemWrite = "0";
        string Jump = "0";
        string MEMSig = Branch + MemRead + MemWrite + Jump;

        string RegDest = "1";
        string ALUop1 = "1";
        string ALUop2 = "0";
        string ALUsrc = "0";
        string EXSig = RegDest + ALUop1 + ALUop2 + ALUsrc;

        ID_EX[0][0] = WBSig;
        ID_EX[1][0] = MEMSig;
        ID_EX[2][0] = EXSig;
    }
    else if( instruction == "100011" ) // lw ---- ? -----
    {
        string RegWrite = "1";
        string MemtoReg = "1";
        string WBSig = RegWrite + MemtoReg;

        string Branch = "0";
        string MemRead = "1";
        string MemWrite = "0";
        string Jump = "0";
        string MEMSig = Branch + MemRead + MemWrite + Jump;

        string RegDest = "0";
        string ALUop1 = "0";
        string ALUop2 = "0";
        string ALUsrc = "1";
        string EXSig = RegDest + ALUop1 + ALUop2 + ALUsrc;

        ID_EX[0][0] = WBSig;
        ID_EX[1][0] = MEMSig;
        ID_EX[2][0] = EXSig;
    }
    else if( instruction == "101011" ) // sw
    {
        string RegWrite = "0";
        string MemtoReg = "x";
        string WBSig = RegWrite + MemtoReg;

        string Branch = "0";
        string MemRead = "0";
        string MemWrite = "1";
        string Jump = "0";
        string MEMSig = Branch + MemRead + MemWrite + Jump;

        string RegDest = "x";
        string ALUop1 = "0";
        string ALUop2 = "0";
        string ALUsrc = "1";
        string EXSig = RegDest + ALUop1 + ALUop2 + ALUsrc;

        ID_EX[0][0] = WBSig;
        ID_EX[1][0] = MEMSig;
        ID_EX[2][0] = EXSig;
    }
    else if( instruction == "000100" ) // beq
    {
        string RegWrite = "0";
        string MemtoReg = "x";
        string WBSig = RegWrite + MemtoReg;

        string Branch = "1";
        string MemRead = "0";
        string MemWrite = "0";
        string Jump = "0";
        string MEMSig = Branch + MemRead + MemWrite + Jump;

        string RegDest = "x";
        string ALUop1 = "0";
        string ALUop2 = "1";
        string ALUsrc = "0";
        string EXSig = RegDest + ALUop1 + ALUop2 + ALUsrc;

        ID_EX[0][0] = WBSig;
        ID_EX[1][0] = MEMSig;
        ID_EX[2][0] = EXSig;
    }
    else if( instruction == "000010" ) // jump
    {
        string RegWrite = "0";
        string MemtoReg = "0";
        string WBSig = RegWrite + MemtoReg;

        string Branch = "0";
        string MemRead = "0";
        string MemWrite = "0";
        string Jump = "1";
        string MEMSig = Branch + MemRead + MemWrite + Jump;

        string RegDest = "0";
        string ALUop1 = "0";
        string ALUop2 = "0";
        string ALUsrc = "0";
        string EXSig = RegDest + ALUop1 + ALUop2 + ALUsrc;

        ID_EX[0][0] = WBSig;
        ID_EX[1][0] = MEMSig;
        ID_EX[2][0] = EXSig;
    }
}

string ALUControl(string FuncCode, string ALUOp)
{
    if(ALUOp == "00")
    {
        return "0010";
    }
    else if(ALUOp == "01")
    {
        return "0110";
    }
    else if(ALUOp == "10")
    {
        if(FuncCode == "100000")
        {
            return "0010";
        }
        else if(FuncCode == "100010")
        {
            return "0110";
        }
        else if(FuncCode == "100100")
        {
            return "0000";
        }
        else if(FuncCode == "100101")
        {
            return "0001";
        }
        else if(FuncCode == "101010")
        {
            return "0111";
        }
    }
}

string ALU(string op1, string op2, string ALUsig)
{
    long long int op1i = binaryToInt(op1,1,32);
    long long int op2i = binaryToInt(op2,1,32);
    long long int res;
    if(ALUsig == "0010")
    {
        res = op1i + op2i;
    }
    else if(ALUsig == "0110")
    {
        watch(op1i);
        watch(op2i);
        res = op1i - op2i;
    }
    else if(ALUsig == "0000")
    {
        res = op1i & op2i;
    }
    else if(ALUsig == "0001")
    {
        res = op1i | op2i;
    }
    else if(ALUsig == "0111")
    {
        res = (op1i < op2i) ? 1 : 0;
    }

    return intToBinary(res);
}

char globalPCsrc = '0';

bool IF() // okok
{
    long long int tempPC = binaryToInt(PCreg[1],0) + 1; //add +1 to pc
    string pc_1 = intToBinary(tempPC);
    IF_ID[0][0] = pc_1;
    long long int cur_pc = tempPC - 1;
    IF_ID[2][0] = intToBinary(cur_pc);


    if(cur_pc >= instructionMemory.size())
    {
        IF_ID[1][0] = zerosx32;
        return false;
    }

    string instruction = instructionMemory[binaryToInt(PCreg[1],0)];

    // check for syscall
    if(instruction == "10101010101010101010101010101010") {
        IF_ID[1][0] = zerosx32;
        return false;
    }

    PCreg[0] = intToBinary(tempPC);
    IF_ID[1][0] = instruction;

    return true;
}

// prototype
void hazard_control();

bool ID()
{
    ID_EX[3][0] = IF_ID[0][1];          // PC_1         //_______CHECK THIS______
    ID_EX[11][0] = IF_ID[2][1];          // PC

    string instruction = IF_ID[1][1];

    if(instruction == zerosx32 or instruction == "11111111111111111111111111111111")
    {
        ID_EX[0][0] = "00";
        ID_EX[1][0] = "0000";
        ID_EX[2][0] = "0000";
        ID_EX[4][0] = zerosx32;
        ID_EX[5][0] = zerosx32;
        ID_EX[6][0] = zerosx32;
        ID_EX[7][0] = "00000";
        ID_EX[8][0] = "00000";
        return false;
    }

    string controlString = instruction.substr(0,6); // opcode
    CONTROL(controlString);
    int rs = binaryToInt(instruction.substr(6,5),0);
    string data1 = registers[rs];
    int rt = binaryToInt(instruction.substr(11,5),0);
    string data2 = registers[rt];
    string rd_1 = instruction.substr(16,5); // for what ??
    string rd_2 = instruction.substr(11,5); // for what ??
    string constant32 = signExtend(instruction.substr(16,16));

    string jumpAddress = IF_ID[0][1].substr(0,6) + instruction.substr(6,26);

    ID_EX[4][0] = data1;
    ID_EX[5][0] = data2;
    ID_EX[6][0] = constant32;
    ID_EX[7][0] = rd_1; // write register rd
    ID_EX[8][0] = rd_2; // write data rt
    ID_EX[9][0] = instruction.substr(6,5); // rs
    ID_EX[10][0] = jumpAddress;

    return true;
} // okok

int forwarding_unit() {
    // check for data hazards ex
    // Regwrite
    if((EX_MEM[0][1][0] == '1' ) //and EX_MEM[][1].RegisterRd ≠ 0)
       and (EX_MEM[6][1] == ID_EX[9][1])) {
        //          rd           rs
        return 1;
    }

    if((EX_MEM[0][1][0] == '1' ) //and EX_MEM[][1].RegisterRd ≠ 0)
       and (EX_MEM[6][1] == ID_EX[8][1])) {
        //          rd           rt
        return 2;
    }

    // check for mem
    //  regwrite
    if ((MEM_WB[0][1][0] == '1') // and (MEM/WB.RegisterRd ≠ 0)

        and (not ( (EX_MEM[0][1][0] == '1') // and (EX_MEM[][1].RegisterRd ≠ 0) extra
             and (EX_MEM[6][1] == ID_EX[9][1]) ))

        and (MEM_WB[3][1] == ID_EX[9][1])) {
        return 3;
    }

    if ((MEM_WB[0][1][0] == '1') // and (MEM/WB.RegisterRd ≠ 0)

        and (not ( (EX_MEM[0][1][0] == '1') // and (EX_MEM[][1].RegisterRd ≠ 0) extra
                   and (EX_MEM[6][1] == ID_EX[8][1]) ))

        and (MEM_WB[3][1] == ID_EX[8][1])) {
        return 4;
    }
}

// TODO : ---------------------
// kuch GALAT H bracnhing m, dekh liyo

void hazard_control() {
    if(
            // Memread
            ID_EX[1][1][1] == '1' and
                        // rt                 rs
            (
                    (ID_EX[8][1] == IF_ID[1][1].substr(6,5)) or
                            // rt             rt
                    (ID_EX[8][1] == IF_ID[1][1].substr(11,5))
             )
             ) {
        cout << "------------------------------stall--------------" << endl;
        ID_EX[0][0] = "00";
        ID_EX[1][0] = "0000";
        ID_EX[2][0] = "0000";
        ID_EX[4][0] = zerosx32;
        ID_EX[5][0] = zerosx32;
        ID_EX[6][0] = zerosx32;
        ID_EX[7][0] = "00000";
        ID_EX[8][0] = "00000";
        ID_EX[9][0] = "00000";
        //ID_EX[10][0] = zerosx32;

        //PCWrite
        for(int i=0;i<3;i++)
            IF_ID[i][0] = IF_ID[i][1];

        PCreg[0] = PCreg[1];
    }
}

// WBSig; MEMSig; EXSig; PC+1; DATA1; DATA2; Constant32; RD_1; RD_2; PC
// WBSig; MEMSig; NewPC; ALUzero; ALUresult; WriteData; DestReg; PC
bool EX()
{
    EX_MEM[0][0] = ID_EX[0][1];   //WBSig
    EX_MEM[1][0] = ID_EX[1][1];   //MEMSig
    EX_MEM[7][0] = ID_EX[10][1];  // jaddr
    EX_MEM[8][0] = ID_EX[11][1];   //PC
                                                // pc+1                              // offset
    string NewPC = intToBinary(binaryToInt(ID_EX[3][1],0) + binaryToInt(ID_EX[6][1],1,32)); // NewPc

    string ALUsig = ALUControl(ID_EX[6][1].substr(26,6), ID_EX[2][1].substr(1,2));
    string op1 = ID_EX[4][1];
    string op2 = (ID_EX[2][1][3] == '1') ? ID_EX[6][1] : ID_EX[5][1]; // operand2

    int x = forwarding_unit(); // add

    cout << "------------------------------------------asdsadsadsa---------------" << endl;
    watch(x);

    if(x == 1) {
        //     ALUresult
        op1 = EX_MEM[4][1];
    }
    else if(x == 2) {
        op2 = EX_MEM[4][1];
    }
    else if(x == 3) {
        if(MEM_WB[0][1][1]=='1')
        {
            op1 = MEM_WB[1][1];
        }
        else
        {
            op1 = MEM_WB[2][1];
        }
    }
    else if(x == 4) {
        if(MEM_WB[0][1][1]=='1')
        {
            op2 = MEM_WB[1][1];
        }
        else
        {
            op2 = MEM_WB[2][1];
        }
    }

    string ALUresult = ALU(op1, op2, ALUsig);
    string ALUzero = (ALUresult == zerosx32) ? "1" : "0";
    string DestReg = (ID_EX[2][1][0] == '0') ? ID_EX[8][1] : ID_EX[7][1]; //TODO : change here ~~~~~~


    EX_MEM[2][0] = NewPC;
    EX_MEM[3][0] = ALUzero;
    EX_MEM[4][0] = ALUresult;
    EX_MEM[5][0] = ID_EX[5][1];
    EX_MEM[6][0] = DestReg;


    // When there is no opcode, ALUop is "10" meaning there should be some sub or add
    // but since there is no func code, metans that there is no insr.
    // func code                                           // ALUop = Aluop1 + ALUop2
    if(ID_EX[6][1].substr(26,6) == "000000" and ID_EX[0][1] == "00" and ID_EX[1][1] == "0000" and ID_EX[2][1] == "0000") {
        cout << "23850239999999999999999958023985029385028350982039580928350928350982309582039580239850298350928305982309582035" << endl;
        EX_MEM[0][0][0] = '0'; // make regwrite zero, since there is no operation being done/
        return false;
    }
    return true;                      //--------------RETURN FALSE WHEN???????----
}

int cntr = -1;
void reset() {
    for(int i=0;i<3;i++) IF_ID[i][0] = IF_ID[i][1];
    for(int i=0;i<12;i++) ID_EX[i][0] = ID_EX[i][1];
    for(int i=0;i<9;i++) EX_MEM[i][0] = EX_MEM[i][1];
    for(int i=0;i<5;i++) MEM_WB[i][0] = MEM_WB[i][1];
}

// WBSig; MEMSig; NewPC; ALUzero; ALUresult; WriteData; DestReg; PC
// WBSig; ReadData; ALUresult; DestReg; PC
bool MEM()
{

    MEM_WB[0][0] = EX_MEM[0][1]; // WBSig
    MEM_WB[2][0] = EX_MEM[4][1]; // ALUresult
    MEM_WB[3][0] = EX_MEM[6][1]; // DestReg
    MEM_WB[4][0] = EX_MEM[8][1]; // PC

    string ReadData = zerosx32;
    int ok=0;
        // MemSig                // ALUzero
    if(EX_MEM[1][1][0] == '1' && EX_MEM[3][1] == "1") // branching
    {
        PCreg[0] = EX_MEM[2][1]; ok++; // next instr must be done

        // NOW CLEAN/FLUSH THE PREVIOUS PIPELINE
        for(int i=0;i<3;i++) IF_ID[i][0] = zerosx32;
        for(int i=0;i<12;i++) ID_EX[i][0] = zerosx32;
        for(int i=0;i<9;i++) EX_MEM[i][0] = zerosx32;
    }

    if(EX_MEM[1][1][3] == '1')  // jump
    {
        PCreg[0] = EX_MEM[7][1]; ok++;
        for(int i=0;i<3;i++) IF_ID[i][0] = zerosx32;
        for(int i=0;i<12;i++) ID_EX[i][0] = zerosx32;
        for(int i=0;i<9;i++) EX_MEM[i][0] = zerosx32;
    }

        // MemSig
    if(EX_MEM[1][1][1] == '1')
    {
        cout << "&&&&&&&&&&&&& \"&&&&&&&&&&&&&\" \"&&&&&&&&&&&&&\" \"&&&&&&&&&&&&&\" \"&&&&&&&&&&&&&\" \"&&&&&&&&&&&&&\" " << endl;
        watch(cntr);
        cout << endl;
        cout << endl;

        if(cntr == 0) {
            ReadData = dataMemory[binaryToInt(EX_MEM[4][1],0)]; ok++;
            cntr--;
        }
        else if(cntr > 0) {
            cntr--;
            reset();
            return true;
        }
        else if(cntr == -1){

            // rand here
            int p = rand() % 100;

            // HIT
            if (p <= X) {
                ReadData = dataMemory[binaryToInt(EX_MEM[4][1], 0)]; ok++;
            } else {
                cntr = N-1;
                reset();
                return true;
            }
        }
    }   // MemSig
    else if(EX_MEM[1][1][2] == '1')
    {
        dataMemory[binaryToInt(EX_MEM[4][1],0)] = EX_MEM[5][1]; ok++;
    }

    if(EX_MEM[0][1] == "10") ok++;  // for add/sub cases since they will done in next stage.

    MEM_WB[1][0] = ReadData;
    if(ok) return true;
    else return false;                  //----------------FALSE WHEN????????-----------
}

// WBSig; ReadData; ALUresult; DestReg; PC
bool WB()
{
    string WriteData = zerosx32;
    int ok=0;
    if(MEM_WB[0][1][1]=='1')
    {
        WriteData = MEM_WB[1][1];
    }
    else
    {
        WriteData = MEM_WB[2][1];
    }

    if(MEM_WB[0][1][0] == '1')
    {
        registers[binaryToInt(MEM_WB[3][1],0)] = WriteData; ok++;
    }

    if(ok) return true;
    else return false; //--------------FALSE WHEN?????????-------------
}

void refreshPipelineRegisters() // okok
{
    for(int i = 0; i < 3; i++)
    {
        IF_ID[i][1] = IF_ID[i][0];
    }

    for(int i = 0; i < 12; i++)
    {
        ID_EX[i][1] = ID_EX[i][0];
    }

    for(int i = 0; i < 9; i++)
    {
        EX_MEM[i][1] = EX_MEM[i][0];
    }

    for(int i = 0; i < 5; i++)
    {
        MEM_WB[i][1] = MEM_WB[i][0];
    }

    PCreg[1] = PCreg[0];
}

bool clockForward()
{
    cout << "------------- \"-------------\" \"-------------\" \"-------------\"" << endl;
    watch(clockCount);
    cout << "------------- \"-------------\" \"-------------\" \"-------------\"" << endl;
    refreshPipelineRegisters();

    bool ret1 = IF();
    print_IF_ID();

    bool ret2 = WB();
    print_registers();

    bool ret3 = ID();
    print_ID_EX();

    // Check for hazards here, must come here, cos we want overwrite ID_EX, IF_ID, PCReg
    hazard_control();  // lw
//    forwarding_unit(); // add

    bool ret4 = EX();
    print_EX_MEM();

    bool ret5 = MEM();
    print_MEM_WB();

    print_registers();
    print_memory();

    clockCount++;

    cout << "bool values : " << ret1 << " " << ret2 << " " <<ret3 << " " <<ret4 << " " <<ret5 << " " << endl;
    return (ret1 | ret2 | ret3 | ret4 | ret5);
}

void init() {
    PCreg[0] = zerosx32;
    PCreg[1] = zerosx32;
    for(int i=0;i<32;i++) registers[i] = intToBinary(i);
    dataMemory[3] = intToBinary(3);
//    registers[1] = intToBinary(1);
}

int main()
{
    // set seeds
    srand(time(nullptr));

    init();

    string line;
    string word;

    while(getline(cin, line)){
        vector <string> instruc;
        istringstream check(line);
        check >> word;
        instructionMemory.push_back(word);
    }
    watch(instructionMemory);

    while(clockForward());
//    int res = 0;
//    for(int i=0;i<17;i++) {
//        res = clockForward();
//        if(!res) break;
//    }
    cout << "Everything done !!" << endl;
    return 0;
}

/*
 *
 *  ass 8 correct wrt 10
 *  ass 9 correct wrt 10
 *  demonstrate x and N
 *  compare 8 and 9 clock cycles
 *  create 3 test cases
 *
 */
