#!/bin/bash

premake5 gmake && make &&
	valgrind --leak-check=full --track-origins=yes ./bin/Debug/testmuzzy
