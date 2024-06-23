class CyclicBuffer
{
private:
    int length;     // Size of the buffer
    int *buffer;  // Pointer to the buffer array
    int head;     // Index of the next insertion
    int *bufferArray; // Ordered array

    void inOrder(){
        int i = 0;
        for (int j = head; j < length; j++)
        {
            bufferArray[i] = buffer[j];
            i++;
        }
        for (int j = 0; j < head; j++)
        {
            bufferArray[i] = buffer[j];
            i++;
        }
    }

public:
    // Constructor: Initializes the buffer with the given size
    CyclicBuffer(int length)
    {
        this->length = length;
        buffer = new int[length];
        bufferArray = new int[length];
        for (int i = 0; i < length; ++i)
        {
            buffer[i] = 0;
            bufferArray[i] = 0;
        }
        head = 0;
    }

    // Destructor: Deallocates the buffer memory
    ~CyclicBuffer()
    {
        delete[] buffer;
    }

    // Push a value into the buffer
    void push(int value)
    {
        buffer[head] = value; // Insert value at the head
        head = (head + 1) % length; // Move head to the next position
    }

    // Get the buffer size in bytes
    size_t sizeBytes()
    {
        return length * sizeof(int);
    }

    // Get the buffer size
    size_t size()
    {
        return length;
    }

    // Print the array in hex
    void printHex() // this prints 0 to length -- should be tail to head
    {
        inOrder();
        for (int i = 0; i < length; i++)
        {
            Serial.printf("%04X", bufferArray[i]);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Print the array
    void print() // this prints 0 to length -- should be tail to head
    {
        inOrder();
        for (int i = 0; i < length; i++)
        {
            Serial.print(bufferArray[i]);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Clear the buffer
    void clear()
    {
        head = 0;
        for (int i = 0; i < length; ++i)
        {
            buffer[i] = 0;
        }
    }

    void toByteArray(byte *byteArray)
    {
        inOrder();
        memcpy(byteArray, bufferArray, sizeof(bufferArray));
    }
};
