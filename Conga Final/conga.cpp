#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <chrono>

using namespace std;

struct GameState{
    vector<string> posMoves; //collection of moves made
    string colour; //from fen
    int moveNo; //from fen
    int Score; //from making a evaluate
    string board[7][7]; //from FEN
    string bw[7][7]; //fROM FEN
    vector<vector<int>> wPawns; //PAWNS
    vector<vector<int>> bPawns; //PAWNS
    vector<vector<int>> swPawns; //SPAWNS
    vector<vector<int>> sbPawns; //SPAWNS
    string state; //FROM MAKE MOVE
    string FEN; //INPUT/FROM BOARD
    bool win = false;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string convert(int x, int y){
    int a = x+1;
    string b;

    if(y==0){
        b = "a";
    }
    if(y==1){
        b = "b";
    }
    if(y==2){
        b = "c";
    }
    if(y==3){
        b = "d";
    }
    if(y==4){
        b = "e";
    }
    if(y==5){
        b = "f";
    }
    if(y==6){
        b = "g";
    }
    string s = b + to_string(a);
    return s;
}
vector<int> convertMove(char y, char x){
    vector<int> temp;
    //cout << x <<  y;
    if(x == '1'){
        temp.push_back(0);
    }
    if(x == '2'){
        temp.push_back(1);
    }
    if(x == '3'){
        temp.push_back(2);
    }
    if(x == '4'){
        temp.push_back(3);
    }
    if(x == '5'){
        temp.push_back(4);
    }
    if(x == '6'){
        temp.push_back(5);
    }
    if(x == '7'){
        temp.push_back(6);
    }

    if(y == 'a'){
        temp.push_back(0);
    }
    if(y == 'b'){
        temp.push_back(1);
    }
    if(y == 'c'){
        temp.push_back(2);
    }
    if(y == 'd'){
        temp.push_back(3);
    }
    if(y == 'e'){
        temp.push_back(4);
    }
    if(y == 'f'){
        temp.push_back(5);
    }
    if(y == 'g'){
        temp.push_back(6);
    }

    return temp;
}

bool checkOpen(GameState gs, int x, int y){ //assume it is in the board
    if(gs.bw[x][y] != gs.colour || gs.bw[x][y] == ""){
        return true;
    }
    else return false;
}

bool isEnemy(GameState gs, int x, int y){ //assume it is in the board
    if(gs.bw[x][y] == "b" && gs.colour == "w"){
        return true;
    }
    else if(gs.bw[x][y] == "w" && gs.colour == "b"){
        return true;
    }
    else return false;
}

//===================================================================================

void visualBoard(GameState gs){
    for(int i = 6; i >= 0; i--){
        cout << "|";
        for(int j = 0; j < 7; j++){
            if(gs.board[i][j] == ""){
                cout << " |";
            }
            else{
                cout << gs.board[i][j] << "|";
            }
        }
        cout << "\n";
    }
}

GameState createBoard(GameState gs){

    for(int i = 0; i < 7 ; i++){
        for(int j = 0; j < 7; j++){
            gs.board[i][j] = "";
            gs.bw[i][j] = "";
        }
    }

    for(int i = 0; i < 7; i++){
        fill(begin(gs.board[i]), end(gs.board[i]), 0);
        fill(begin(gs.bw[i]), end(gs.bw[i]), 0);
    }

    //cout << str;
    //str = trim(str);
    string str = gs.FEN;

    int pos = 0;
    int x = 0;
    //string arrBoard[7][7];
    for(int i = 6; i > -1; i--){
        for(int j = 0; j < 7; j++){
            if(str[pos] == '1' || str[pos] == '2' || str[pos] == '3' || str[pos] == '4' || str[pos] == '5' || str[pos] == '6' || str[pos] == '7'){
                x = str[pos] - '0';
                j += x - 1;
            }
            else if(str[pos] != '/'){
                gs.board[i][j] = str[pos];
                //cout << str[pos] << "\n";
                //j -= 1;
            }
            else{
                j -= 1;
            }
            pos++;
        }
    }

    return gs;

}

GameState assignWhite(GameState gs){

    char alph[7] = {'a','b','c','d','e','f','g'};

    string c;
    vector<int> pw;
    vector<int> spw;

    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 7; j++){
            c = gs.board[j][i];
            if(c == "P"){
                gs.bw[j][i] = "w";
                pw.push_back(j);
                pw.push_back(i);
                gs.wPawns.push_back(pw);
                pw.clear();
            }
            else if(c == "S"){
                gs.bw[j][i] = "w";
                spw.push_back(j);
                spw.push_back(i);
                gs.swPawns.push_back(spw);
                spw.clear();
            }
            else if(isupper(c[0])){
                gs.bw[j][i] = "w";
            }

        }
    }
    return gs;

}

GameState assignBlack(GameState gs){

    string c;
    vector<int> pb;
    vector<int> spb;

    for(int i = 0; i < 7; i++){
        for(int j = 0; j < 7; j++){
            c = gs.board[j][i];
            if(c == "p"){

                gs.bw[j][i] = "b";
                pb.push_back(j);
                pb.push_back(i);
                gs.bPawns.push_back(pb);
                pb.clear();
            }
            else if(c == "s"){
                gs.bw[j][i] = "b";
                spb.push_back(j);
                spb.push_back(i);
                gs.sbPawns.push_back(spb);
                spb.clear();
            }
            else if(islower(c[0])){
                gs.bw[j][i] = "b";
            }

        }
    }

    return gs;

}

vector<string> lionMove(GameState gs){

    int x, y, ex, ey;

    if(gs.colour == "w"){
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 7; j++){
                if(gs.board[i][j] == "L"){
                     x = i;
                     y = j;
                }
                if(gs.board[i][j] == "l"){
                     ex = i;
                     ey = j;
                }
            }
        }
    }
    else{
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 7; j++){
                if(gs.board[i][j] == "l"){
                     x = i;
                     y = j;
                }
                if(gs.board[i][j] == "L"){
                     ex = i;
                    ey = j;
                }
            }
        }
    }

    int startMoves[8][2] = {{x-1, y-1}, {x, y-1}, {x+1, y-1}, {x-1, y}, {x+1, y}, {x-1, y+1}, {x, y+1}, {x+1, y+1}};
    string lionBoard[7][7];
    for(int i = 0; i < 7; i++){
        for(int j = 0; j< 7; j++){
            lionBoard[i][j] == "";
        }
    }

    int currX;
    int currY;
    string moves = "";

    for(int a = 0; a < 8; a++){
            currX = startMoves[a][0];
            currY = startMoves[a][1];

            if(currX >= 0 && currX <= 2 || currX >= 4 && currX <= 6){
                if(currY >= 2 && currY <= 4){
                    //valid
                    if(checkOpen(gs, currX, currY)){
                        lionBoard[currX][currY] = "a";
                    }

                }
            }
    }

    //check enemy lion
    bool win = true;

    if(gs.colour == "w"){
        if(y == ey){
            for(int i = x+1; i < ex; i++){
                if(!checkOpen(gs, i, y) || isEnemy(gs, i, y)){
                    win = false;
                }
            }
        }
        else if(checkOpen(gs, 3,3) && !isEnemy(gs, 3, 3) && x == 2 && ex == 4 && y != 3 && ey != 3){ //if diagonal not blocked
            win = true;
        }
        else {

            win = false;
        }
    }

    if(gs.colour == "b"){
        if(y == ey){
            for(int i = x-1; i > ex; i--){
                if(!checkOpen(gs, i, y) || isEnemy(gs, i, y)){
                    win = false;
                }
            }
        }
        else if(checkOpen(gs, 3,3) && !isEnemy(gs, 3, 3) && x == 4 && ex == 2 && y != 3 && ey != 3){
            win = true;
        }
        else{
            win = false;
        }
    }

    if (win){
        lionBoard[ex][ey] = "a";
    }

    vector<string> lion;
    for(int i = 0; i < 7; i++){
        for(int j = 0; j< 7; j++){
            if(lionBoard[j][i] == "a"){
                lion.push_back(convert(x, y) + convert(j, i));
            }
        }
    }

    return lion;

}

vector<string> zebraMove(GameState gs){
    int x = -1;
    int y = -1;
    int ex = -1;
    int ey = -1;
    string temp = "";
    vector<string> zebra;

    if(gs.colour == "w"){
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 7; j++){
                if(gs.board[i][j] == "Z"){
                     x = i;
                     y = j;
                }
                if(gs.board[i][j] == "z"){
                     ex = i;
                     ey = j;
                }
            }
        }
    }
    else{
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 7; j++){
                if(gs.board[i][j] == "z"){
                     x = i;
                     y = j;
                }
                if(gs.board[i][j] == "Z"){
                     ex = i;
                    ey = j;
                }
            }
        }
    }
    if(x != -1 && y != -1){
        int startMoves[8][2] = {{x-1, y-2}, {x+1, y-2}, {x-2, y-1}, {x+2, y-1}, {x-2, y+1}, {x+2, y+1}, {x-1, y+2}, {x+1, y+2}};
        string zebraBoard[7][7];
        for(int i = 0; i < 7; i++){
            fill(begin(zebraBoard[i]), end(zebraBoard[i]), 0);
        }
        for(int i = 0; i < 7; i++){
            for(int j = 0; j< 7; j++){
                zebraBoard[i][j] == "";
            }
        }

        int currX;
        int currY;
        string moves = "";

        for(int a = 0; a < 8; a++){
                currX = startMoves[a][0];
                currY = startMoves[a][1];

                if(currX >= 0 && currX <= 6 && currY >= 0 && currY <= 6){
                    //valid
                    if(checkOpen(gs , currX, currY)){
                        zebraBoard[currX][currY] = "a";
                        //cout << "Valid at: " << currX << ":" <<currY << "\n";
                    }
                }
        }
        for(int i = 0; i < 7; i++){
            for(int j = 0; j< 7; j++){
                if(zebraBoard[j][i] == "a"){
                    zebra.push_back(convert(x, y) + convert(j, i));
                }
            }
        }
    }

    return zebra;
}

vector<string> giraffeMove(GameState gs){
    int x = -1;
    int ex = -1;
    int y = -1;
    int ey = -1;
    string temp = "";
    vector<string> giraffe;

    if(gs.colour == "w"){
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 7; j++){
                if(gs.board[i][j] == "G"){
                     x = i;
                     y = j;
                }
                if(gs.board[i][j] == "g"){
                     ex = i;
                     ey = j;
                }
            }
        }
    }
    else{
        for(int i = 0; i < 7; i++){
            for(int j = 0; j < 7; j++){
                if(gs.board[i][j] == "g"){
                     x = i;
                     y = j;
                }
                if(gs.board[i][j] == "G"){
                     ex = i;
                    ey = j;
                }
            }
        }
    }
    if(x != -1 && y != -1){
        int startMoves[16][2] = {{x-2, y-2}, {x, y-2}, {x+2, y-2}, {x-1, y-1},
                                {x, y-1},{x+1, y-1}, {x-2, y}, {x-1, y}, {x+1, y}, {x+2, y},
                                {x-1, y+1}, {x, y+1}, {x+1, y+1}, {x-2, y+2}, {x, y+2}, {x+2, y+2}};


        string girBoard[7][7];
        for(int i = 0; i < 7; i++){
            for(int j = 0; j< 7; j++){
                gs.board[i][j] == "";
            }
        }

        int currX= 0;
        int currY = 0;
        string moves = "";

        for(int a = 0; a < 16; a++){
                currX = startMoves[a][0];
                currY = startMoves[a][1];

                if(currX >= 0 && currX <= 6 && currY >= 0 && currY <= 6){
                    if(checkOpen(gs, currX, currY)){
                        if(isEnemy(gs, currX, currY)){
                            if(currX == x+2 || currY == y+2 || currX == x-2 || currY == y-2){
                                girBoard[currX][currY] = "a";
                            }
                        }
                        else{
                            girBoard[currX][currY] = "a";
                        }
                    }
                }
        }
        for(int i = 0; i < 7; i++){
            for(int j = 0; j< 7; j++){
                if(girBoard[j][i] == "a"){
                    giraffe.push_back(convert(x, y) + convert(j, i));
                }
            }
        }
    }
    return giraffe;

}

vector<string> pawnMove(GameState gs){ //singluar pawn

    string temp = "";
    string pawnBoard[7][7];
    vector<string> pawns;

    for(int i = 0; i < 7; i++){
        for(int j = 0; j< 7; j++){
            pawnBoard[i][j] == "";
        }
    }

    if(gs.colour == "w"){ //is white
        for(auto v : gs.wPawns){
            int x = v.at(0);
            int y = v.at(1);

           // cout << x << ":" << y << "\n";

            if(x>3){ // can move 5 times
                int startMoves[5][2] = {{x+1, y-1}, {x+1, y}, {x+1, y+1}, {x-1, y},{x-2, y}};
                for(int i = 0; i < 3; i++){
                    int currX = startMoves[i][0];
                    int currY = startMoves[i][1];
                    if(checkOpen(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                        pawnBoard[currX][currY] = "a";
                    }
                }
                int currX = startMoves[3][0];
                int currY = startMoves[3][1];
                if(checkOpen(gs, currX, currY) && !isEnemy(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                    pawnBoard[currX][currY] = "a";
                    currX = startMoves[4][0];
                    currY = startMoves[4][1];

                    if(checkOpen(gs, currX, currY) && !isEnemy(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                       pawnBoard[currX][currY] = "a";
                    }
                }
            }
            else{ // can move 3 times
                int startMoves[3][2] = {{x+1, y-1}, {x+1, y}, {x+1, y+1}};
                for(int i = 0; i < 3; i++){
                    int currX = startMoves[i][0];
                    int currY = startMoves[i][1];
                    if(checkOpen(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                        pawnBoard[currX][currY] = "a";
                    }
                }
            }
            for(int i = 0; i < 7; i++){
                for(int j = 0; j< 7; j++){
                    if(pawnBoard[j][i] == "a"){
                        pawns.push_back(convert(x, y) + convert(j, i));
                        pawnBoard[j][i] = "";
                    }
                }
            }
            //cout << convert(v[0], v[1]) << "\n";

        }
    }
    else{ //is black
        for(auto v : gs.bPawns){
            int x = v.at(0);
            int y = v.at(1);

           // cout << x << ":" << y << "\n";

            if(x<3){ // can move 5 times
                int startMoves[5][2] = {{x-1, y-1}, {x-1, y}, {x-1, y+1}, {x+1, y},{x+2, y}};
                for(int i = 0; i < 3; i++){
                    int currX = startMoves[i][0];
                    int currY = startMoves[i][1];
                    if(checkOpen(gs, currX, currY)&& (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                        pawnBoard[currX][currY] = "a";
                    }
                }
                int currX = startMoves[3][0];
                int currY = startMoves[3][1];
                if(checkOpen(gs, currX, currY) && !isEnemy(gs, currX, currY)&& (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                    pawnBoard[currX][currY] = "a";
                    currX = startMoves[4][0];
                    currY = startMoves[4][1];

                    if(checkOpen(gs, currX, currY) && !isEnemy(gs, currX, currY)&& (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                       pawnBoard[currX][currY] = "a";
                    }
                }
            }
            else{ // can move 3 times
                int startMoves[3][2] = {{x-1, y-1}, {x-1, y}, {x-1, y+1}};
                for(int i = 0; i < 3; i++){
                    int currX = startMoves[i][0];
                    int currY = startMoves[i][1];
                    if(checkOpen(gs, currX, currY)&& (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                        pawnBoard[currX][currY] = "a";
                    } // g5c4 g5d4 g5e4
                }
            }
            for(int i = 0; i < 7; i++){
                for(int j = 0; j< 7; j++){
                    if(pawnBoard[j][i] == "a"){
                        pawns.push_back(convert(x, y) + convert(j, i));
                        pawnBoard[j][i] = "";
                    }
                }
            }
            //cout << convert(v[0], v[1]) << "\n";

        }

    }

    return pawns;

}

vector<string> superpawnMove(GameState gs){ //singluar pawn
    string temp = "";
    string spawnBoard[7][7];
    vector<string> sups;

    for(int i = 0; i < 7; i++){
        for(int j = 0; j< 7; j++){
            spawnBoard[i][j] == "";
        }
    }

    if(gs.colour == "w"){ //is white
        for(auto v : gs.swPawns){
            int x = v.at(0);
            int y = v.at(1);

           // cout << x << ":" << y << "\n";

            int startMoves[11][2] = {{x+1, y-1}, {x+1, y}, {x+1, y+1}, {x, y-1}, {x, y+1}, {x-1, y-1}, {x-1, y}, {x-1, y+1}, {x-2, y-2}, {x-2, y}, {x-2, y+2}};
                for(int i = 0; i < 5; i++){
                    int currX = startMoves[i][0];
                    int currY = startMoves[i][1];
                    if(checkOpen(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                        spawnBoard[currX][currY] = "a";
                    }
                }
                for(int i = 5; i < 8; i++){
                    int currX = startMoves[i][0];
                    int currY = startMoves[i][1];
                    if(checkOpen(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7) && !isEnemy(gs, currX, currY)){
                        spawnBoard[currX][currY] = "a";
                        currX = startMoves[i+3][0];
                        currY = startMoves[i+3][1];
                        if(checkOpen(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7) && !isEnemy(gs, currX, currY)){
                            spawnBoard[currX][currY] = "a";
                        }
                    }
                }

            //makling the string===================================================
            for(int i = 0; i < 7; i++){
                for(int j = 0; j< 7; j++){
                    if(spawnBoard[j][i] == "a"){
                        sups.push_back(convert(x, y) + convert(j, i));
                        spawnBoard[j][i] = "";
                    }
                }
            }

        }

    }
    else{ //is black
        for(auto v : gs.sbPawns){
            int x = v.at(0);
            int y = v.at(1);

           // cout << x << ":" << y << "\n";

            int startMoves[11][2] = {{x-1, y-1}, {x-1, y}, {x-1, y+1}, {x, y-1}, {x, y+1}, {x+1, y-1}, {x+1, y}, {x+1, y+1}, {x+2, y-2}, {x+2, y}, {x+2, y+2}};
                for(int i = 0; i < 5; i++){
                    int currX = startMoves[i][0];
                    int currY = startMoves[i][1];
                    if(checkOpen(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7)){
                        spawnBoard[currX][currY] = "a";
                    }
                }
                for(int i = 5; i < 8; i++){
                    int currX = startMoves[i][0];
                    int currY = startMoves[i][1];
                    if(checkOpen(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7) && !isEnemy(gs, currX, currY)){
                        spawnBoard[currX][currY] = "a";
                        currX = startMoves[i+3][0];
                        currY = startMoves[i+3][1];
                        if(checkOpen(gs, currX, currY) && (currX > -1 && currX < 7 && currY > -1 && currY < 7) && !isEnemy(gs, currX, currY)){
                            spawnBoard[currX][currY] = "a";
                        }
                    }
                }

            //makling the string===================================================
            for(int i = 0; i < 7; i++){
                for(int j = 0; j< 7; j++){
                    if(spawnBoard[j][i] == "a"){
                        sups.push_back(convert(x, y) + convert(j, i));
                        spawnBoard[j][i] = "";
                    }
                }
            }
        }

    }

    return sups;


}

GameState getFen(GameState gs){

    //visualBoard(gs);

    string Fen = "";

    int c;

    for(int i =6; i > -1; i--){
        c = 0;
        for(int j = 0; j < 7; j++){
            if(gs.bw[i][j] != "b" && gs.bw[i][j] != "w"){
                c++;
                if(j == 6){
                    Fen += to_string(c);
                }
            }
            else if(c == 0){
                Fen += gs.board[i][j];
            }
            else{
                Fen += to_string(c);
                Fen += gs.board[i][j];
                c=0;
            }
        }
    if(i != 0){
        Fen += "/";
    }

    }

    if(gs.colour == "w"){
        Fen+= " b ";
    }
    else{
        Fen+= " w " ;
        gs.moveNo +=1;
    }
    //check for winnered

    bool win = true;
    if(gs.colour == "w"){
        for(int i = 6; i > -1; i--){
            for(int j = 0; j < 7; j++){
                if(gs.board[i][j] == "l"){
                    win = false;
                }
            }
        }
    }
    if(gs.colour == "b"){
        for(int i = 6; i > -1; i--){
            for(int j = 0; j < 7; j++){
                if(gs.board[i][j] == "L"){
                    win = false;
                }
            }
        }
    }

    Fen += to_string(gs.moveNo);

    if(gs.colour == "w" && win){
        //gs.win = true;
        gs.state = ("White wins");
    }
    else if(gs.colour == "b" && win){
        gs.state = ("Black wins");
        //gs.win = true;
    }
    else{
        gs.state = ("Continue");
    }
    if(win){
        gs.win = true;
    }
    gs.FEN = Fen;

    return gs;

}

vector<string> allMoves(GameState gs){
    vector<string> lion = lionMove(gs);
    vector<string> zebra = zebraMove(gs);
    vector<string> giraffe = giraffeMove(gs);
    vector<string> pawn = pawnMove(gs);
    vector<string> spawn = superpawnMove(gs);

    vector<string> all;

    all.insert(end(all), begin(lion), end(lion));
    all.insert(end(all), begin(zebra), end(zebra));
    all.insert(end(all), begin(giraffe), end(giraffe));
    all.insert(end(all), begin(pawn), end(pawn));
    all.insert(end(all), begin(spawn), end(spawn));

    //gs.posMoves = all;

    return all;
}

int getScore(GameState gs){

    bool wLion = false;
    bool bLion = false;
    if(gs.colour == "w"){ //looking for white win
        for(int i = 6; i > -1; i--){
            for(int j = 0; j<7; j++){
                if(gs.board[i][j] == "L"){
                    wLion = true;
                }
                if(gs.board[i][j] == "l"){
                    bLion = true;
                }
            }
        }

        if(wLion && !bLion){
            return 10000;
        }
        if(bLion && !wLion){
            return -10000;
        }
        if(!wLion && !bLion){
            return 0;
        }
    }

    if(gs.colour == "b"){ //looking for white win
        for(int i = 6; i > -1; i--){
            for(int j = 0; j<7; j++){
                if(gs.board[i][j] == "L"){
                    wLion = true;
                }
                if(gs.board[i][j] == "l"){
                    bLion = true;
                }
            }
        }

        if(wLion && !bLion){
            return -10000;
        }
        if(bLion && !wLion){
            return 10000;
        }
        if(!wLion && !bLion){
            return 0;
        }
    }

    int totalScore = 0;

    for(int i = 6; i > -1; i--){
        for(int j = 0; j < 7; j++){
            string c = gs.board[i][j];

            if(c == "P"){
               totalScore += 100;
            }
            if(c == "p"){
                totalScore -= 100;
            }

            if(c == "Z"){
                totalScore += 300;
            }
            if(c == "z"){
                totalScore -= 300;
            }

            if(c == "S"){
                totalScore += 350;
            }
            if(c == "s"){
                totalScore -= 350;
            }

            if(c == "G"){
                totalScore += 400;
            }
            if(c == "g"){
                totalScore -= 400;
            }
        }
    }

    if(gs.colour == "b"){
        return -1*(totalScore);
    }
    else{
        return totalScore;
    }
}

GameState generateState(GameState gs){

    GameState newState = getFen(gs);
    newState = assignBlack(newState);
    newState = assignWhite(newState);
    vector<string> temp = allMoves(newState);
    newState.Score = getScore(newState);
    temp.clear();

    return newState;
}

GameState makeMove(GameState gs, string moves){

    vector<int> starting = convertMove(moves[0], moves[1]);
    //cout << starting.at(0) <<  starting.at(1);
    int sx = starting.at(0);
    int sy = starting.at(1);

    vector<int> ending = convertMove(moves[2], moves[3]);

    int ex = ending.at(0);
    int ey = ending.at(1);

    string piece = gs.board[sx][sy];
    gs.board[ex][ey] = piece;


    for(int i = 0; i < 7; i++){
        if(gs.bw[3][i] == gs.colour){
            gs.bw[3][i] = "";
            gs.board[3][i] = "";
        }
    }
    //remove all those pieces since they wont be leaving the rive


    //======================================================
    gs.board[sx][sy] = "";

    //========================================================
    if(gs.colour == "w"){
        gs.bw[ex][ey] = "w";
    }
    else{
        gs.bw[ex][ey] = "b";
    }
    gs.bw[sx][sy] = "";

    //check if piece started and ended in river
    if(sx == ex && ex == 3){
        gs.bw[ex][ey] = "";
        gs.board[ex][ey] = "";
    }
    //check white superpawns
    if(gs.colour == "w"){
        gs.colour = "b";
        if(gs.board[ex][ey] == "P" && ex == 6){
            gs.board[ex][ey] = "S";
        }
    }
    else{
        gs.colour = "w";
        if(gs.board[ex][ey] == "p" && ex == 0){
            gs.board[ex][ey] = "s";
        }
    }

    GameState r = generateState(gs);
    return r;

}

bool GameOver(GameState gs){
    bool wLion = false;
    bool bLion = false;
    for(int i = 6; i > -1; i--){
            for(int j = 0; j<7; j++){
                if(gs.board[i][j] == "L"){
                    wLion = true;
                }
                if(gs.board[i][j] == "l"){
                    bLion = true;
                }
            }
        }

    if(wLion && !bLion){
        return true;
    }
    if(!wLion && bLion){
        return true;
    }
    if(!wLion && !bLion){
        return true;
    }

    return false;
}

int minimax(GameState currState, int depth){

    if(GameOver(currState) || depth <= 0){
        return getScore(currState);
    }
    int value = -1000000;
    vector<string> moves = allMoves(currState);
    for(auto m : moves){
        GameState newState = makeMove(currState, m);
        int eval = (-1)*(minimax(newState, depth -1));
        value = max(value, eval);
    }
    return value;

}

int main(){
    int N;
    cin >> N;
    cin.ignore();

    for (int i = 0; i < N; ++i) {
        string fen;
        //string moves;
        getline(cin, fen);
       // getline(cin, moves);

        vector<string> sFEN{};

        size_t pos = 0;
        while ((pos = fen.find(" ")) != string::npos) {
            sFEN.push_back(fen.substr(0, pos));
            fen.erase(0, pos + 1);
        }
        sFEN.push_back(fen.substr(0, pos));


        string sboard = sFEN[0];
        string toPlay = sFEN[1];
        int moveNo = stoi(sFEN[2]);

        GameState startState;
        startState.colour = toPlay;
        startState.FEN = sboard;
        startState.moveNo = moveNo;

        startState = createBoard(startState);
        startState = assignBlack(startState);
        startState = assignWhite(startState);

        visualBoard(startState);

       cout << minimax(startState, 2) << "\n";
    }

    return 0;

}
