#include "backprop.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
namespace { struct P { double w=.5,b=0,v=1,c=0; }; struct G { double w,b,v,c; }; constexpr double x=1,y=2,lr=.1;
double loss(const P&p){double h=std::tanh(p.w*x+p.b),e=p.v*h+p.c-y;return .5*e*e;}
G grad(const P&p){double h=std::tanh(p.w*x+p.b),e=p.v*h+p.c-y,d=e*p.v*(1-h*h);return {d*x,d,e*h,e};}
void step(P&p,const G&g){p.w-=lr*g.w;p.b-=lr*g.b;p.v-=lr*g.v;p.c-=lr*g.c;}
double numeric(P p,int i){double* z= i==0?&p.w:i==1?&p.b:i==2?&p.v:&p.c;*z+=1e-6;double a=loss(p);*z-=2e-6;return (a-loss(p))/(2e-6);}
}
bool run_backpropagation(const std::string& directory,std::string& report) { P p;G g=grad(p); double diffs[4]={std::abs(g.w-numeric(p,0)),std::abs(g.b-numeric(p,1)),std::abs(g.v-numeric(p,2)),std::abs(g.c-numeric(p,3))};for(double d:diffs)if(d>1e-5){report="Finite-difference gradient check failed.";return false;} std::ofstream out(directory+"/backprop_loss.csv");std::ofstream deriv(directory+"/backprop_derivation.txt");if(!out||!deriv){report="Cannot create backprop output.";return false;}out<<"step,loss\n";for(int i=0;i<=100;++i){out<<i<<','<<std::setprecision(12)<<loss(p)<<'\n';if(i<100){G q=grad(p);step(p,q);}}deriv<<"x=1, target=2; h=tanh(wx+b); yhat=vh+c; J=0.5(yhat-y)^2\ninitial w=0.5,b=0,v=1,c=0, learning rate=0.1\ninitial loss="<<loss(P{})<<"\ngradients: dJ/dw="<<g.w<<", dJ/db="<<g.b<<", dJ/dv="<<g.v<<", dJ/dc="<<g.c<<"\ncentral finite differences epsilon=1e-6 absolute errors: "<<diffs[0]<<","<<diffs[1]<<","<<diffs[2]<<","<<diffs[3]<<"\nchain rule: dJ/dyhat=e; dJ/dv=e*h; dJ/dc=e; dJ/dw=e*v*(1-h^2)*x; dJ/db=e*v*(1-h^2).\nAll four parameters update simultaneously from the original gradients.\n";report="100-step loss CSV and analytical/finite-difference evidence exported.";return true; }
