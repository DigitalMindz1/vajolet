/*
	This file is part of Vajolet.

    Vajolet is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Vajolet is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Vajolet.  If not, see <http://www.gnu.org/licenses/>
*/

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <iterator>
#include "vajolet.h"
#include "command.h"
#include "io.h"
#include "position.h"
#include "movegen.h"
#include "move.h"





const static char* StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
/*	\brief print the uci reply
	\author Marco Belli
	\version 1.0
	\date 21/10/2013
*/
void static printUciInfo(void){
	sync_cout<< "id name "<<PROGRAM_NAME<<" "<<VERSION<<std::endl;
	std::cout<<"id author Belli Marco"<<std::endl;
	std::cout<<"uciok"<<sync_endl;
}

Move moveFromUci(Position& pos, std::string& str) {

	Movegen mg;
	mg.generateMoves<Movegen::allMg>(pos);
	for(unsigned int i=0;i< mg.getGeneratedMoveNumber();i++){
		if(str==pos.displayUci(mg.getGeneratedMove(i))){
			return mg.getGeneratedMove(i);
		}
	}
	Move m;
	m.packed=0;
	return m;
}

void static position(std::istringstream& is, Position & pos){
	std::string token, fen;
	is >> token;
	if (token == "startpos")
	{
		fen = StartFEN;
		is >> token; // Consume "moves" token if any
	}
	else if (token == "fen")
		while (is >> token && token != "moves")
			fen += token + " ";
	else
		return;
	pos.setupFromFen(fen);

	Move m;
	// Parse move list (if any)
	while (is >> token && (m = moveFromUci(pos, token)).packed != 0)
	{
		pos.doMove(m);
	}
}


/*	\brief manage the uci loop
	\author Marco Belli
	\version 1.0
	\date 21/10/2013
*/
void uciLoop(){
	Position pos;
	pos.setupFromFen(StartFEN);
	std::string token, cmd;

/*	Move m1,m2,m3,m4,m5,m6,m7;
	//pos.display();
	m1.packed=0;
	m1.from=A2;
	m1.to=A4;
	pos.doMove(m1);
	//pos.display();
	m2.packed=0;
	m2.from=G8;
	m2.to=F6;
	pos.doMove(m2);
	//pos.display();
	m3.packed=0;
	m3.from=A4;
	m3.to=A5;
	pos.doMove(m3);
	//pos.display();
	m4.packed=0;
	m4.from=B7;
	m4.to=B5;
	pos.doMove(m4);
	//pos.display();
	m5.packed=0;
	m5.from=A5;
	m5.to=B6;
	m5.flags=Move::fenpassant;
	pos.doMove(m5);
	pos.display();
	m6.from=F8;
	m6.to=C5;
	pos.doMove(m6);
	//pos.display();
	m7.from=E1;
	m7.to=G1;
	m7.flags= Move::fcastle;
	pos.doMove(m7);
	//pos.display();
	pos.undoMove(m7);
	pos.undoMove(m6);
	pos.undoMove(m5);
	pos.undoMove(m4);
	pos.undoMove(m3);
	pos.undoMove(m2);
	pos.undoMove(m1);
	pos.display();*/


	do{
		if (!std::getline(std::cin, cmd)) // Block here waiting for input
			cmd = "quit";
		std::istringstream is(cmd);

		is >> std::skipws >> token;


		if(token== "uci"){
			printUciInfo();
		}
		else if (token =="quit" || token =="stop" || token =="ponderhit"){
		}
		else if (token =="ucinewgame"){
		}
		else if (token =="d"){
			pos.display();
			Movegen m;
			m.generateMoves<Movegen::allMg>(pos);
		}
		else if (token =="position"){
			position(is,pos);
		}
		else if (token =="eval"){
			sync_cout<<"Eval:" <<pos.eval()/10000.0<<sync_endl;
		}
		else if (token =="isready"){
			sync_cout<<"readyok"<<sync_endl;
		}
		else if (token =="perft" && (is>>token)){

			unsigned long elapsed = std::chrono::duration_cast<std::chrono::milliseconds >(std::chrono::steady_clock::now().time_since_epoch()).count();
			unsigned long res=pos.perft(stoi(token));
			elapsed = std::chrono::duration_cast<std::chrono::milliseconds >(std::chrono::steady_clock::now().time_since_epoch()).count()-elapsed;
			sync_cout<<"perft Res="<<res<<" "<<elapsed<<"ms"<<sync_endl;
			sync_cout<<((double)res)/elapsed<<"kN/s"<<sync_endl;


		}
		else if (token =="divide" && (is>>token)){

			unsigned long res=pos.divide(stoi(token));
			sync_cout<<"divide Res="<<res<<sync_endl;
		}
		else{
			sync_cout<<"unknown command"<<sync_endl;
		}
	}while(token!="quit");
}
