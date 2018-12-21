all : client server server_select check

client : util.o client.o
	gcc -o client client.o util.o

server : util.o server.o
	gcc -o server server.o util.o

server_select : util.o server_select.o
	gcc -o server_select server_select.o util.o

check : check.o
	gcc -o check check.o

client.o : client.c
	gcc -c -o client.o client.c

server.o : server.c
	gcc -c -o server.o server.c

server_select.o : server_select.c
	gcc -c -o server_select.o server_select.c

check.o : check.c
	gcc -c -o check.o check.c

util.o : util.c
	gcc -c -o util.o util.c

clean :
	rm *.o client server check

test :
	./client -h 143.248.56.16 -p 5000 -o 0 -s 5 < ./test-cases/test-results_dec/1KB_dec.txt > ./test-cases/self-test-results/1KB_from_dec.txt
	./client -h 143.248.56.16 -p 5000 -o 1 -s 5 < ./test-cases/test-results_enc/1KB_enc.txt > ./test-cases/self-test-results/1KB_from_enc.txt
	./client -h 143.248.56.16 -p 5000 -o 0 -s 0 < ./test-cases/test-vector/1KB.txt > ./test-cases/self-test-results/1KB_from_vector.txt
	./client -h 143.248.56.16 -p 5000 -o 0 -s 5 < ./test-cases/test-results_dec/1MB_dec.txt > ./test-cases/self-test-results/1MB_from_dec.txt
	./client -h 143.248.56.16 -p 5000 -o 1 -s 5 < ./test-cases/test-results_enc/1MB_enc.txt > ./test-cases/self-test-results/1MB_from_enc.txt
	./client -h 143.248.56.16 -p 5000 -o 0 -s 0 < ./test-cases/test-vector/1MB.txt > ./test-cases/self-test-results/1MB_from_vector.txt
	./client -h 143.248.56.16 -p 5000 -o 0 -s 5 < ./test-cases/test-results_dec/20MB_dec.txt > ./test-cases/self-test-results/20MB_from_dec.txt
	./client -h 143.248.56.16 -p 5000 -o 1 -s 5 < ./test-cases/test-results_enc/20MB_enc.txt > ./test-cases/self-test-results/20MB_from_enc.txt
	./client -h 143.248.56.16 -p 5000 -o 0 -s 0 < ./test-cases/test-vector/20MB.txt > ./test-cases/self-test-results/20MB_from_vector.txt
	./check 1KB_from_dec.txt 1KB_from_enc.txt 1KB_from_vector.txt
	./check 1MB_from_dec.txt 1MB_from_enc.txt 1MB_from_vector.txt
	./check 20MB_from_dec.txt 20MB_from_enc.txt 20MB_from_vector.txt

test_local :
	./client -h 127.0.0.1 -p 4001 -o 0 -s 5 < ./test-cases/test-results_dec/1KB_dec.txt > ./test-cases/self-test-results/1KB_from_dec.txt
	./client -h 127.0.0.1 -p 4001 -o 1 -s 5 < ./test-cases/test-results_enc/1KB_enc.txt > ./test-cases/self-test-results/1KB_from_enc.txt
	./client -h 127.0.0.1 -p 4001 -o 0 -s 0 < ./test-cases/test-vector/1KB.txt > ./test-cases/self-test-results/1KB_from_vector.txt
	./client -h 127.0.0.1 -p 4001 -o 0 -s 5 < ./test-cases/test-results_dec/1MB_dec.txt > ./test-cases/self-test-results/1MB_from_dec.txt
	./client -h 127.0.0.1 -p 4001 -o 1 -s 5 < ./test-cases/test-results_enc/1MB_enc.txt > ./test-cases/self-test-results/1MB_from_enc.txt
	./client -h 127.0.0.1 -p 4001 -o 0 -s 0 < ./test-cases/test-vector/1MB.txt > ./test-cases/self-test-results/1MB_from_vector.txt
	./client -h 127.0.0.1 -p 4001 -o 0 -s 5 < ./test-cases/test-results_dec/20MB_dec.txt > ./test-cases/self-test-results/20MB_from_dec.txt
	./client -h 127.0.0.1 -p 4001 -o 1 -s 5 < ./test-cases/test-results_enc/20MB_enc.txt > ./test-cases/self-test-results/20MB_from_enc.txt
	./client -h 127.0.0.1 -p 4001 -o 0 -s 0 < ./test-cases/test-vector/20MB.txt > ./test-cases/self-test-results/20MB_from_vector.txt
	./check 1KB_from_dec.txt 1KB_from_enc.txt 1KB_from_vector.txt
	./check 1MB_from_dec.txt 1MB_from_enc.txt 1MB_from_vector.txt
	./check 20MB_from_dec.txt 20MB_from_enc.txt 20MB_from_vector.txt
