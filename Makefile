test:
	gcc -m32 -o pthread_test pthread.c pthread_test.c
	./pthread_test
	
clean:
	rm pthread_test

