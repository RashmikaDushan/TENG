class CyclicBuffer
{
private:
    int size;     // Size of the buffer
    int *buffer;  // Pointer to the buffer array
    int head;     // Index of the next insertion
    int tail;     // Index of the next removal
    int valCount; // Count of the current elements in the buffer

public:
    // Constructor: Initializes the buffer with the given size
    CyclicBuffer(int size)
    {
        this->size = size;
        buffer = new int[size];
        head = 0;
        tail = 0;
        valCount = 0;
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

        // If the buffer is full, move the tail to the next position
        if (valCount == size)
        {
            tail = (tail + 1) % size;
        }
        else
        {
            valCount++; // Otherwise, increase the element count
        }

        head = (head + 1) % size; // Move head to the next position
    }

    // Pop a value from the buffer
    int pop()
    {
        // If the buffer is empty, return a placeholder value
        if (isEmpty())
        {
            return -1; // Placeholder value for an empty buffer
        }

        int value = buffer[tail]; // Get the value at the tail
        tail = (tail + 1) % size; // Move tail to the next position
        valCount--;               // Decrease the element count
        return value;
    }

    // Check if the buffer is full
    bool isFull()
    {
        return valCount == size;
    }

    // Check if the buffer is empty
    bool isEmpty()
    {
        return valCount == 0;
    }

    // Get the buffer size in bytes
    size_t bufferSizeBytes()
    {
        return size*sizeof(int);
    }

    // Get the buffer size
    size_t bufferSize()
    {
        return size;
    }

    // Print the array in hex
    void printHex(){
        for (int i = 0; i < size; i++)
        {
            Serial.printf("%X",buffer[i]);
            Serial.print(" ");
        }
        Serial.println();
    }

    // Find the maximum value in the buffer
    int buffmax()
    {
        // If the buffer is empty, return a placeholder value
        if (isEmpty())
        {
            return -1; // Placeholder value for an empty buffer
        }

        int maxVal = buffer[tail];         // Start with the value at the tail
        for (int i = 1; i < valCount; i++) // Iterate over the valid elements
        {
            int index = (tail + i) % size; // Calculate the current index
            if (buffer[index] > maxVal)    // Update maxVal if a larger value is found
            {
                maxVal = buffer[index];
            }
        }
        return maxVal; // Return the maximum value found
    }

    // Get the current head position
    int getHead()
    {
        return head;
    }

    // Peek the value at the tail without removing it
    int peek()
    {
        // If the buffer is not empty, return the value at the tail
        if (!isEmpty())
        {
            return buffer[tail];
        }
        else
        {
            return -1; // Placeholder value for an empty buffer
        }
    }

    // Get the value at a specific index
    int get(int index)
    {
        // If the index is within valid range, return the corresponding value
        if (index >= 0 && index < valCount)
        {
            return buffer[(tail + index) % size];
        }
        else
        {
            return -1; // Placeholder value for an invalid index
        }
    }

    // Clear the buffer
    void clear()
    {
        head = 0;
        tail = 0;
        valCount = 0; // Reset the element count
    }

    void toByteArray(byte * byteArray)
    {
        memcpy(byteArray, buffer, sizeof(buffer));
    }
};
