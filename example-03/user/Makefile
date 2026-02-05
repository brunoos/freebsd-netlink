.PHONY: clean

T=nl_echo_client
S=nl_echo_client.c

$(T): $(S)
	$(CC) -Wall -Werror -O2 -o $@ $^

clean:
	rm -f *.o $(T)
