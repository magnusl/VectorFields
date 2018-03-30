#include <vf_proto\vf.h>
#include <vf_proto\intermediate.hpp>

#include <iostream>

using namespace std;

const char * pSource = "in vec4 a;in vec4 b;uniform float r;out accumulate vec4 c;void main() {c = normalize(a) + b*r;}";

int main()
{
    float a[10000];
    float b[10000];
    float c[10000];

    for(size_t i = 0; i < 10000; i++) {
        a[i] = float(i);
        b[i] = sqrtf(float(i));
        c[i] = 0;
    }

    static uint8_t buf[1024*100];

    try {
        std::shared_ptr<vf::Program> program = std::make_shared<vf::Program>(pSource);
        program->SetUniform("r", 2.0);

        std::shared_ptr<vf::ByteCode> bytecode = program->Compile();
        vf::ByteCode_Execution exec(bytecode, buf, sizeof(buf));
        exec.SetRegisterPointer(bytecode->StreamLocation("a"), a);
        exec.SetRegisterPointer(bytecode->StreamLocation("b"), b);
        exec.SetRegisterPointer(bytecode->StreamLocation("c"), c);

        vf::Status_t err = exec.Execute(0, 2500);
        err = exec.Execute(0, 2500);
    } catch (std::runtime_error& err) {
        cerr << err.what() << endl;
    }
}