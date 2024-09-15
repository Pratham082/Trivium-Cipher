#include <bits/stdc++.h>

using namespace std;
const int key_size = 80;
const int IV_size = 80;
const int state_size = 288;
const int pre_run_cycles = 1152;
unordered_map<char, string> hexToBinMap = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
        {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
        {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"},
        {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"}
    };
unordered_map<string, char> BinTohexMap = {
        {"0000", '0'}, {"0001", '1'}, {"0010", '2'}, {"0011", '3'},
        {"0100", '4'}, {"0101", '5'}, {"0110", '6'}, {"0111", '7'},
        {"1000", '8'}, {"1001", '9'}, {"1010", 'A'}, {"1011", 'B'},
        {"1100", 'C'}, {"1101", 'D'}, {"1110", 'E'}, {"1111", 'F'}
    };


string reverse_(string str){ // function for reversing the input key and IV as required
    string new_str = "";
    for (int i=2;i<str.size();i++){
        if (str[i] == ' ') continue;
        new_str += str[i];
    }
    stack<char> st;
    for (int i = 0; i < 20; i += 2) {
        st.push(new_str[i + 1]);
        st.push(new_str[i]);
    }

    int i = 0;
    while (!st.empty()) {
        char p = st.top(); st.pop();
        char q = st.top(); st.pop();
        new_str[i++] = p;
        new_str[i++] = q;
    }
    return "0x"+new_str;

}

bitset<key_size> HextoBinary(string str){ // function for conversion of Hexadecimal to BInary representation
    bitset<key_size> new_bitstream;
    string binary_str = ""; 
    for (int i=2;i<str.size();i++){
        if (str[i] == ' ') continue; 
        binary_str += hexToBinMap[str[i]];
    }
    reverse(binary_str.begin(), binary_str.end());
    // cout << "2. "<<binary_str<< endl;
    istringstream(binary_str) >> new_bitstream;
    return new_bitstream;
}

string BinarytoHex(bitset<512> keystream){ // function for conversion of Binary to Hexadecimal representation   
    string Hex_Keystream = "";
    int i=0;
    while ( i<512){
        string binary = "";
        for (int j=0;j<4;j++){
            binary += to_string(keystream[i+j]);
        }   
        i += 4;
        reverse(binary.begin(), binary.end());
        Hex_Keystream += BinTohexMap[binary];
    }
    for (int i=0;i<Hex_Keystream.size();i+=2){
        swap(Hex_Keystream[i], Hex_Keystream[i+1]);
    }
    string final_keystream = "";
    for (int i=0;i<Hex_Keystream.size();i++){
        if (i%4 ==0 && i!=0) final_keystream += " ";
        final_keystream += Hex_Keystream[i];
    }
    return "0x"+final_keystream;
}

class Trivium{ // class declaration for representing the Trivium cipher
    private: 
        bitset<288> s;
        bitset<93> s1; // declaring the register 1 
        bitset<84> s2; // declaring the register 2
        bitset<111> s3; // declaring the register 3
        bitset<512> ans; // declaring the bitstream answer which will contain the 512 bits generated as the answer to the question.

    public:
   
        Trivium(const bitset<key_size> &key, const bitset<IV_size> &IV){
             /* constructor function which initiated the three registers with 
                first register s1 with key bits
                2nd register s2 with IV bits, and 
                the rest bits of the 108 bits with 0 ending with 111
                */
            for (int i =0;i<80;i++){
                this->s[i] = key[i];
            }
            for (int i=80;i<93;i++){
                this->s[i] = 0;
            }
            int j=0;
            for (int i =93;i<173;i++){
                this->s[i] = IV[j++]; 
            }
            for (int i=173;i<177;i++){
                this->s[i] = 0;
            }
            for (int i=177;i<285;i++){
                this->s[i] = 0;
            }
            this->s[285] = 1;
            this->s[286] = 1;
            this->s[287] = 1;

        }
        void update(bool t1, bool t2, bool t3){ // updating and shifting the bits in register after each clock cycle
            for (int i=92;i>0;i--){
                s[i] = s[i-1];
            }
            s[0] = t3;
            for (int i=176;i>93;i--){
                s[i] = s[i-1];
            }
            s[93] = t1;
            for (int i=287;i>177;i--){
                s[i] = s[i-1];
            }
            s[177] = t2;
        }
        void pre_run(){ // function to pre_run the cipher model, for 1152 times.
            for (int i=0;i<pre_run_cycles;i++){
                bool o1 = (s[65] ^ s[92]);
                bool o2 = (s[161] ^ s[176]);
                bool o3 = (s[242] ^ s[287]);
                bool t1 = o1 ^ (s[90] & s[91]) ^ s[170];
                bool t2 = o2 ^ (s[174] & s[175]) ^ s[263];
                bool t3 = o3 ^ (s[285] & s[286]) ^ s[68];

                update(t1,t2,t3);
            }
        }
        bitset<1> generate_keystream_bit(){ // generating one bit keystream for one clock time.
            bool o1 = (s[65] ^ s[92]);
            bool o2 = (s[161] ^ s[176]);
            bool o3 = (s[242] ^ s[287]);
            bool t1 = o1 ^ (s[90] & s[91]) ^ s[170];
            bool t2 = o2 ^ (s[174] & s[175]) ^ s[263];
            bool t3 = o3 ^ (s[285] & s[286]) ^ s[68];
            bool out = o1^o2^o3;

            update(t1,t2,t3);

            return out;
        }
        void generate_keystream(){ // generating the keystream for 512 bits.
            for (int i=0;i<512;i++){
                this->ans[i] = this->generate_keystream_bit()[0];
            }
        }
        void show_keystream(){
           cout << BinarytoHex(this->ans);
        }

};




int main(){
    cout << "Enter the Key: "<<endl;
    string key;
    getline(cin, key);
    cout << "Enter the IV: "<< endl;
    string IV;
    getline(cin, IV);
    key = reverse_(key);
    IV = reverse_(IV);

    // cout << key<<endl;
    bitset<80> Binary_key = HextoBinary(key);
    bitset<80> Binary_IV = HextoBinary(IV);

    // cout << "Key in Binary"<<endl;
    // for (int i=0;i<80;i++){
    //     cout << Binary_key[i];
    // }
    // cout << endl;
    // cout << "IV in binary"<<endl;
    // for (int i=0;i<80;i++){
    //     cout << Binary_IV[i];
    // }

    // cout << BinarytoHex(Binary_key)<< endl;
    // cout << BinarytoHex(Binary_IV)<<endl;

    Trivium Cipher(Binary_key, Binary_IV); // Defining the Cipher object as Trivium, giving the Binary_Key and Binary_IV

    Cipher.pre_run(); // warming up the cipher, by running it for 1152 cycles.

    Cipher.generate_keystream(); // generating the keystream bits as asked in the question.
    Cipher.show_keystream(); //output the 512 bits of keystream, which was created in the previous step.  
}
