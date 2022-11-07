TARGET = directree

$(TARGET): main.c
	$(CC) main.c -o $(TARGET)

clean:
	$(RM) $(TARGET)