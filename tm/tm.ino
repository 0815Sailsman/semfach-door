
char band[] = {
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
        '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'
        };
short position = 1;

bool tm(char input[]) {
    const char NONE = 'N';
    const char LEFT = 'L';
    const char RIGHT = 'R';

    short state = 0;
    write_input_to_band(input);
    short final_state = 42;

    char read_value;
    char write_value;
    int next_state;
    char next_move;
    while (true) {
      //Serial.println(band);
       read_value = band[position];
       //Serial.print("read");
       //Serial.println(read_value);
       write_value = get_write_value(state, read_value);
       if (write_value == '-1') {
           return false;
       }
       //Serial.print("write");
       //Serial.println(write_value);
       next_state = get_next_state(state, read_value);
       if (next_state == final_state) {
           return true;
       }
       else if (next_state == -1) {
           return false;
       }
       //Serial.print("state");
       //Serial.println(next_state);
        next_move = get_next_move(state, read_value);
        if (next_move == '-1') {
            return false;
        }
       //Serial.print("move");
       //Serial.println(next_move);
        band[position] = write_value;
        state = next_state;
        if (next_move == NONE) {
            position = position;
        }
        else if (next_move == RIGHT) {
            position++;
        }
        else if (next_move == LEFT) {
            position--;
        }
    }
}


void write_input_to_band(char input[]) {
    short backup = position;
    for (short i = 0; i < 5; i++) {
        band[position] = input[i];
        position++;
    }
    position = backup;
}

char get_write_value(int state, char read){
    switch (state)
    {
    case 0:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 1:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 2:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 3:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 4:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 5:
        switch (read)
        {
        case '#':
            return '|';
        default:
            break;
        }
    case 6:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 7:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 8:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 9:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 10:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 11:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 12:
        switch (read)
        {
        case '#':
            return '|';
        default:
            break;
        }
    case 13:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        default:
            break;
        }
    case 14:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 15:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        }
    case 16:
        switch (read)
        {
        case '#':
            return '#';
        case '0':
            return '0';
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        }
    case 17:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 18:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '0';
        default:
            break;
        }
    case 19:
        switch (read)
        {
        case '#':
            return '0';
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 20:
        switch (read)
        {
        case '#':
            return '0';
        default:
            break;
        }
    case 21:
        switch (read)
        {
        case '#':
            return '|';
        default:
            break;
        }
    case 22:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 23:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '0';
        default:
            break;
        }
    case 24:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '1';
        default:
            break;
        }
    case 25:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 26:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '2';
        case '2':
            return '3';
        case '3':
            return '4';
        case '4':
            return '5';
        case '5':
            return '6';
        case '6':
            return '7';
        case '7':
            return '8';
        case '8':
            return '9';
        case '9':
            return '0';
        default:
            break;
        }
    case 27:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '2';
        case '2':
            return '3';
        case '3':
            return '4';
        default:
            break;
        }
    case 28:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        default:
            break;
        }
    case 29:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        }   
    case 30:
        switch (read)
        {
        case '0':
            return '0';
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        } 
    case 31:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '2';
        case '2':
            return '3';
        case '3':
            return '4';
        case '4':
            return '5';
        case '5':
            return '6';
        case '6':
            return '7';
        case '7':
            return '8';
        case '8':
            return '9';
        case '9':
            return '0';
        default:
            break;
        } 
    case 32:
        switch (read)
        {
        case '0':
            return '1';
        case '1':
            return '2';
        case '2':
            return '3';
        case '3':
            return '4';
        case '4':
            return '5';
        case '5':
            return '6';
        case '6':
            return '7';
        case '7':
            return '8';
        case '8':
            return '9';
        default:
            break;
        } 
    case 33:
        switch (read)
        {
        case '0':
            return '0';
        default:
            break;
        } 
    case 34:
        switch (read)
        {
        case '0':
            return '0';
        default:
            break;
        } 
    case 35:
        switch (read)
        {
        case '|':
            return '|';
        default:
            break;
        } 
    case 36:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        default:
            break;
        } 
    case 37:
        switch (read)
        {
        case '1':
            return '0';
        case '2':
            return '1';
        case '3':
            return '2';
        case '4':
            return '3';
        case '5':
            return '4';
        case '6':
            return '5';
        case '7':
            return '6';
        case '8':
            return '7';
        case '9':
            return '8';
        default:
            break;
        }
    case 38:
        switch (read)
        {
        case '#':
            return '#';
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        case '2':
            return '2';
        case '3':
            return '3';
        case '4':
            return '4';
        case '5':
            return '5';
        case '6':
            return '6';
        case '7':
            return '7';
        case '8':
            return '8';
        case '9':
            return '9';
        default:
            break;
        }
    case 39:
        switch (read)
        {
        case '|':
            return '|';
        default:
            break;
        }
    case 40:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        default:
            break;
        }
    case 41:
        switch (read)
        {
        case '0':
            return '0';
        default:
            break;
        }
    case 42:
        break;
    case 43:
        switch (read)
        {
        case '|':
            return '|';
        default:
            break;
        }
    case 44:
        switch (read)
        {
        case '|':
            return '|';
        case '0':
            return '0';
        case '1':
            return '1';
        default:
            break;
        }

    default:
        return '-1';
    }
    return '-1';
}

int get_next_state(int state, char read){
    switch (state)
    {
    case 0:
        switch (read)
        {
        case '0':
            return 1;
        case '1':
            return 1;
        case '2':
            return 1;
        case '3':
            return 1;
        case '4':
            return 1;
        case '5':
            return 1;
        case '6':
            return 1;
        case '7':
            return 1;
        case '8':
            return 1;
        case '9':
            return 1;
        default:
            break;
        }
    case 1:
        switch (read)
        {
        case '0':
            return 2;
        case '1':
            return 2;
        case '2':
            return 2;
        case '3':
            return 2;
        case '4':
            return 2;
        case '5':
            return 2;
        case '6':
            return 2;
        case '7':
            return 2;
        case '8':
            return 2;
        case '9':
            return 2;
        default:
            break;
        }
    case 2:
        switch (read)
        {
        case '0':
            return 3;
        case '1':
            return 3;
        case '2':
            return 3;
        case '3':
            return 3;
        case '4':
            return 3;
        case '5':
            return 3;
        case '6':
            return 3;
        case '7':
            return 3;
        case '8':
            return 3;
        case '9':
            return 3;
        default:
            break;
        }
    case 3:
        switch (read)
        {
        case '0':
            return 4;
        case '1':
            return 4;
        case '2':
            return 4;
        case '3':
            return 4;
        case '4':
            return 4;
        case '5':
            return 4;
        case '6':
            return 4;
        case '7':
            return 4;
        case '8':
            return 4;
        case '9':
            return 4;
        default:
            break;
        }
    case 4:
        switch (read)
        {
        case '0':
            return 5;
        case '1':
            return 5;
        case '2':
            return 5;
        case '3':
            return 5;
        case '4':
            return 5;
        case '5':
            return 5;
        case '6':
            return 5;
        case '7':
            return 5;
        case '8':
            return 5;
        case '9':
            return 5;
        default:
            break;
        }
    case 5:
        switch (read)
        {
        case '#':
            return 6;
        default:
            break;
        }
    case 6:
        switch (read)
        {
        case '#':
            return 7;
        default:
            break;
        }
    case 7:
        switch (read)
        {
        case '#':
            return 8;
        default:
            break;
        }
    case 8:
        switch (read)
        {
        case '#':
            return 9;
        default:
            break;
        }
    case 9:
        switch (read)
        {
        case '#':
            return 10;
        default:
            break;
        }
    case 10:
        switch (read)
        {
        case '#':
            return 11;
        default:
            break;
        }
    case 11:
        switch (read)
        {
        case '#':
            return 12;
        default:
            break;
        }
    case 12:
        switch (read)
        {
        case '#':
            return 13;
        default:
            break;
        }
    case 13:
        switch (read)
        {
        case '|':
            return 14;
        case '0':
            return 13;
        case '1':
            return 13;
        default:
            break;
        }
    case 14:
        switch (read)
        {
        case '0':
            return 15;
        case '1':
            return 15;
        case '2':
            return 15;
        case '3':
            return 15;
        case '4':
            return 15;
        case '5':
            return 15;
        case '6':
            return 15;
        case '7':
            return 15;
        case '8':
            return 15;
        case '9':
            return 15;
        default:
            break;
        }
    case 15:
        switch (read)
        {
        case '0':
            return 16;
        case '1':
            return 17;
        case '2':
            return 17;
        case '3':
            return 17;
        case '4':
            return 17;
        case '5':
            return 17;
        case '6':
            return 17;
        case '7':
            return 17;
        case '8':
            return 17;
        case '9':
            return 17;
        default:
            break;
        }
    case 16:
        switch (read)
        {
        case '#':
            return 19;
        case '0':
            return 16;
        case '1':
            return 17;
        case '2':
            return 17;
        case '3':
            return 17;
        case '4':
            return 17;
        case '5':
            return 17;
        case '6':
            return 17;
        case '7':
            return 17;
        case '8':
            return 17;
        case '9':
            return 17;
        default:
            break;
        }
    case 17:
        switch (read)
        {
        case '|':
            return 44;
        case '0':
            return 17;
        case '1':
            return 17;
        case '2':
            return 17;
        case '3':
            return 17;
        case '4':
            return 17;
        case '5':
            return 17;
        case '6':
            return 17;
        case '7':
            return 17;
        case '8':
            return 17;
        case '9':
            return 17;
        default:
            break;
        }
    case 18:
        switch (read)
        {
        case '0':
            return 13;
        case '1':
            return 18;
        default:
            break;
        }
    case 19:
        switch (read)
        {
        case '#':
            return 20;
        case '|':
            return 19;
        case '0':
            return 19;
        case '1':
            return 19;
        case '2':
            return 19;
        case '3':
            return 19;
        case '4':
            return 19;
        case '5':
            return 19;
        case '6':
            return 19;
        case '7':
            return 19;
        case '8':
            return 19;
        case '9':
            return 19;
        default:
            break;
        }
    case 20:
        switch (read)
        {
        case '#':
            return 21;
        default:
            break;
        }
    case 21:
        switch (read)
        {
        case '#':
            return 22;
        default:
            break;
        }
    case 22:
        switch (read)
        {
        case '|':
            return 23;
        case '0':
            return 22;
        case '1':
            return 22;
        case '2':
            return 22;
        case '3':
            return 22;
        case '4':
            return 22;
        case '5':
            return 22;
        case '6':
            return 22;
        case '7':
            return 22;
        case '8':
            return 22;
        case '9':
            return 22;
        default:
            break;
        }
    case 23:
        switch (read)
        {
        case '|':
            return 28;
        case '0':
            return 23;
        case '1':
            return 24;
        default:
            break;
        }
    case 24:
        switch (read)
        {
        case '|':
            return 25;
        case '0':
            return 24;
        default:
            break;
        }
    case 25:
        switch (read)
        {
        case '0':
            return 26;
        case '1':
            return 26;
        case '2':
            return 26;
        case '3':
            return 26;
        case '4':
            return 26;
        case '5':
            return 26;
        case '6':
            return 26;
        case '7':
            return 26;
        case '8':
            return 26;
        case '9':
            return 26;
        default:
            break;
        }
    case 26:
        switch (read)
        {
        case '0':
            return 22;
        case '1':
            return 22;
        case '2':
            return 22;
        case '3':
            return 22;
        case '4':
            return 22;
        case '5':
            return 22;
        case '6':
            return 22;
        case '7':
            return 22;
        case '8':
            return 22;
        case '9':
            return 27;
        default:
            break;
        }
    case 27:
        switch (read)
        {
        case '0':
            return 22;
        case '1':
            return 22;
        case '2':
            return 22;
        case '3':
            return 22;
        default:
            break;
        }
    case 28:
        switch (read)
        {
        case '|':
            return 29;
        case '0':
            return 28;
        default:
            break;
        }
    case 29:
        switch (read)
        {
        case '0':
            return 30;
        case '1':
            return 31;
        case '2':
            return 31;
        case '3':
            return 31;
        case '4':
            return 31;
        case '5':
            return 31;
        case '6':
            return 31;
        case '7':
            return 31;
        case '8':
            return 31;
        case '9':
            return 31;
        default:
            break;
        }   
    case 30:
        switch (read)
        {
        case '0':
            return 34;
        case '1':
            return 33;
        case '2':
            return 33;
        case '3':
            return 33;
        case '4':
            return 33;
        case '5':
            return 33;
        case '6':
            return 33;
        case '7':
            return 33;
        case '8':
            return 33;
        case '9':
            return 33;
        default:
            break;
        } 
    case 31:
        switch (read)
        {
        case '0':
            return 29;
        case '1':
            return 29;
        case '2':
            return 29;
        case '3':
            return 29;
        case '4':
            return 29;
        case '5':
            return 29;
        case '6':
            return 29;
        case '7':
            return 29;
        case '8':
            return 29;
        case '9':
            return 32;
        default:
            break;
        } 
    case 32:
        switch (read)
        {
        case '0':
            return 29;
        case '1':
            return 29;
        case '2':
            return 29;
        case '3':
            return 29;
        case '4':
            return 29;
        case '5':
            return 29;
        case '6':
            return 29;
        case '7':
            return 29;
        case '8':
            return 29;
        default:
            break;
        } 
    case 33:
        switch (read)
        {
        case '0':
            return 35;
        default:
            break;
        } 
    case 34:
        switch (read)
        {
        case '0':
            return 39;
        default:
            break;
        } 
    case 35:
        switch (read)
        {
        case '|':
            return 36;
        default:
            break;
        } 
    case 36:
        switch (read)
        {
        case '|':
            return 37;
        case '0':
            return 36;
        default:
            break;
        } 
    case 37:
        switch (read)
        {
        case '1':
            return 38;
        case '2':
            return 38;
        case '3':
            return 38;
        case '4':
            return 38;
        case '5':
            return 38;
        case '6':
            return 38;
        case '7':
            return 38;
        case '8':
            return 38;
        case '9':
            return 38;
        default:
            return -1;
        }
    case 38:
        switch (read)
        {
        case '#':
            return 43;
        case '|':
            return 38;
        case '0':
            return 38;
        case '1':
            return 38;
        case '2':
            return 38;
        case '3':
            return 38;
        case '4':
            return 38;
        case '5':
            return 38;
        case '6':
            return 38;
        case '7':
            return 38;
        case '8':
            return 38;
        case '9':
            return 38;
        default:
            break;
        }
    case 39:
        switch (read)
        {
        case '|':
            return 40;
        default:
            break;
        }
    case 40:
        switch (read)
        {
        case '|':
            return 41;
        case '0':
            return 40;
        default:
            break;
        }
    case 41:
        switch (read)
        {
        case '0':
            return 42;
        default:
            break;
        }
    case 42:
        break;
    case 43:
        switch (read)
        {
        case '|':
            return 30;
        default:
            break;
        }
    case 44:
        switch (read)
        {
        case '|':
            return 18;
        case '0':
            return 44;
        case '1':
            return 44;
        default:
            break;
        }

    default:
        return -1;
    }
    return -1;
}

char get_next_move(int state, char read){
    switch (state)
    {
    case 0:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 1:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 2:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 3:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 4:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 5:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 6:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 7:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 8:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 9:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 10:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 11:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 12:
        switch (read)
        {
        case '#':
            return 'L';
        default:
            break;
        }
    case 13:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'L';
        case '1':
            return 'L';
        default:
            break;
        }
    case 14:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        }
    case 15:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 16:
        switch (read)
        {
        case '#':
            return 'R';
        case '0':
            return 'L';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 17:
        switch (read)
        {
        case '|':
            return 'R';
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 18:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        default:
            break;
        }
    case 19:
        switch (read)
        {
        case '#':
            return 'R';
        case '|':
            return 'R';
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 20:
        switch (read)
        {
        case '#':
            return 'R';
        default:
            break;
        }
    case 21:
        switch (read)
        {
        case '#':
            return 'L';
        default:
            break;
        }
    case 22:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        }
    case 23:
        switch (read)
        {
        case '|':
            return 'R';
        case '0':
            return 'L';
        case '1':
            return 'R';
        default:
            break;
        }
    case 24:
        switch (read)
        {
        case '|':
            return 'R';
        case '0':
            return 'R';
        default:
            break;
        }
    case 25:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 26:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        }
    case 27:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        default:
            break;
        }
    case 28:
        switch (read)
        {
        case '|':
            return 'R';
        case '0':
            return 'R';
        default:
            break;
        }
    case 29:
        switch (read)
        {
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }   
    case 30:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        } 
    case 31:
        switch (read)
        {
        case '0':
            return 'L';
        case '1':
            return 'L';
        case '2':
            return 'L';
        case '3':
            return 'L';
        case '4':
            return 'L';
        case '5':
            return 'L';
        case '6':
            return 'L';
        case '7':
            return 'L';
        case '8':
            return 'L';
        case '9':
            return 'L';
        default:
            break;
        } 
    case 32:
        switch (read)
        {
        case '0':
            return 'N';
        case '1':
            return 'N';
        case '2':
            return 'N';
        case '3':
            return 'N';
        case '4':
            return 'N';
        case '5':
            return 'N';
        case '6':
            return 'N';
        case '7':
            return 'N';
        case '8':
            return 'N';
        default:
            break;
        } 
    case 33:
        switch (read)
        {
        case '0':
            return 'L';
        default:
            break;
        } 
    case 34:
        switch (read)
        {
        case '0':
            return 'L';
        default:
            break;
        } 
    case 35:
        switch (read)
        {
        case '|':
            return 'L';
        default:
            break;
        } 
    case 36:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'L';
        default:
            break;
        } 
    case 37:
        switch (read)
        {
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 38:
        switch (read)
        {
        case '#':
            return 'L';
        case '|':
            return 'R';
        case '0':
            return 'R';
        case '1':
            return 'R';
        case '2':
            return 'R';
        case '3':
            return 'R';
        case '4':
            return 'R';
        case '5':
            return 'R';
        case '6':
            return 'R';
        case '7':
            return 'R';
        case '8':
            return 'R';
        case '9':
            return 'R';
        default:
            break;
        }
    case 39:
        switch (read)
        {
        case '|':
            return 'L';
        default:
            break;
        }
    case 40:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'L';
        default:
            break;
        }
    case 41:
        switch (read)
        {
        case '0':
            return 'N';
        default:
            break;
        }
    case 42:
        break;
    case 43:
        switch (read)
        {
        case '|':
            return 'L';
        default:
            break;
        }
    case 44:
        switch (read)
        {
        case '|':
            return 'L';
        case '0':
            return 'R';
        case '1':
            return 'R';
        default:
            break;
        }

    default:
        return '-1';
    }
    return '-1';
}

void setup() {
  Serial.begin(9600);
  Serial.println("Los gehts...");
  char input[5] = {'5', '3', '2', '1', '2'};
  bool result = tm(input);
  
  Serial.print("Das Ergebnis lautet: ");
  Serial.println(result);
}

void loop() {

}
