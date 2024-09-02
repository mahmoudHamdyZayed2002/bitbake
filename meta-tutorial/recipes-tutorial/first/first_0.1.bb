DESCRIPTION = "I am the first recipe"
PR = "r2"
do_build () {
	echo "BUILD dir is already fpound"

  	cd ./HMM_S
  	mkdir BUILD
  	echo hi hi hi hi 
  	echo "first: some shell script running as build"
  	gcc -o hmm.o -c hmm_test.c
  	gcc -fPIC -o hmm_pic.o -c hmm_test.c
  	gcc -shared -o libhmm.so hmm_pic.o
  	ar -rs libhmm.a hmm.o
 	mv *.o ./BUILD
  	mv *.a ./BUILD
  	mv *.so ./BUILD
  	mv BUILD ../

  
}
