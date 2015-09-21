
goal: TestIRCServer Client git

TestIRCServer: TestIRCServer.c
	g++ -g -o TestIRCServer TestIRCServer.c

Client: Controller.cc ChatView.cc Comm.cc NewUserView.cc SetupView.cc errorWindow.cc
	g++ -g Controller.cc ChatView.cc Comm.cc  NewUserView.cc SetupView.cc errorWindow.cc -o Client `pkg-config --cflags --libs gtk+-2.0`

git:
	git add -A >> .local.git.out
	git commit -a -m "MakeIRCclient" >> .local.git.out

clean:
	rm -f hello panned entry radio timer TestIRCServer

