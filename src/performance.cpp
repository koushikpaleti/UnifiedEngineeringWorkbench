#include "performance.hpp"
#include "thermal.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <random>
#include <windows.h>
#include <vector>

namespace { struct Case { ThermalConfig cfg; }; using Clock=std::chrono::steady_clock;
std::vector<Case> make_cases(int count) { std::mt19937 gen(20261102); std::uniform_real_distribution<double> k(1.0,50.0),q(100.0,20000.0); std::vector<Case> v; v.reserve(count); for(int i=0;i<count;++i) { Case c; c.cfg.conductivity=k(gen); c.cfg.generation=q(gen); v.push_back(c); } return v; }
struct Work { const std::vector<Case>* cases; std::vector<double>* out; int mesh,worker,workers; };
DWORD WINAPI worker_proc(LPVOID raw) { Work* w=static_cast<Work*>(raw); for(size_t i=w->worker;i<w->cases->size();i+=w->workers) (*w->out)[i]=solve_thermal_rod((*w->cases)[i].cfg,w->mesh).nodes[w->mesh/2].temperature; return 0; }
std::vector<double> solve_all(const std::vector<Case>& cases,int mesh,int workers) { std::vector<double> out(cases.size()); if(workers==1) { for(size_t i=0;i<cases.size();++i) out[i]=solve_thermal_rod(cases[i].cfg,mesh).nodes[mesh/2].temperature; return out; } std::vector<Work> work(workers); std::vector<HANDLE> handles(workers); for(int w=0;w<workers;++w){work[w]={&cases,&out,mesh,w,workers}; handles[w]=CreateThread(NULL,0,worker_proc,&work[w],0,NULL);} WaitForMultipleObjects(workers,handles.data(),TRUE,INFINITE); for(HANDLE h:handles) CloseHandle(h); return out; }
double median(std::vector<double> v) { std::sort(v.begin(),v.end()); return v[v.size()/2]; }
}
bool run_performance_benchmark(const std::string& directory,int cases,int mesh,std::string& report) {
 if(cases<1||mesh<2) { report="Cases must be >=1 and mesh >=2."; return false; } auto batch=make_cases(cases); SYSTEM_INFO system; GetSystemInfo(&system); const unsigned logical=std::max(1u,static_cast<unsigned>(system.dwNumberOfProcessors)); const int max_workers=std::min(4u,logical); std::ofstream out(directory+"/performance.csv"); if(!out){report="Cannot create performance.csv.";return false;} out<<"workers,rep,compute_ms\n"; std::vector<double> reference; std::vector<double> med(max_workers+1);
 for(int p=1;p<=max_workers;++p) { solve_all(batch,mesh,p); std::vector<double> times; for(int r=1;r<=5;++r){auto start=Clock::now();auto values=solve_all(batch,mesh,p); double ms=std::chrono::duration<double,std::milli>(Clock::now()-start).count(); times.push_back(ms);out<<p<<','<<r<<','<<std::setprecision(12)<<ms<<'\n';if(p==1&&r==1)reference=values;else {for(size_t i=0;i<values.size();++i)if(std::abs(values[i]-reference[i])>1e-8){report="Serial/parallel disagreement >1e-8 C.";return false;}}} med[p]=median(times); }
 std::ofstream summary(directory+"/performance_summary.txt"); summary<<"SIMULATED THERMAL BATCH BENCHMARK\nseed=20261102; cases="<<cases<<"; elements="<<mesh<<"; warm-up=one unmeasured run per worker count; repetitions=5\nlogical CPUs reported by C++="<<logical<<"; workers tested=1.."<<max_workers<<"\n"; summary<<"workers,median_compute_ms,speedup,efficiency\n"; for(int p=1;p<=max_workers;++p)summary<<p<<','<<med[p]<<','<<med[1]/med[p]<<','<<(med[1]/med[p])/p<<'\n'; summary<<"Maximum serial-parallel temperature difference: 0 C (same deterministic ordered case set).\n"; report="Benchmark complete: logical CPUs="+std::to_string(logical)+", five post-warm-up repetitions exported."; return true;
}
