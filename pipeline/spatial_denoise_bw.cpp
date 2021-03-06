#include <stdio.h>
#include <math.h>
#include <complex>
#include "Halide.h"
#include "halide_image_io.h"
//#include "denoise.h"
#include <time.h>



using namespace Halide;
using namespace Halide::ConciseCasts;
using namespace std;


int main(int argc, char **argv) {

	clock_t time;
	time = clock();

	Buffer<uint8_t> in_b = Tools::load_image("../images/noise_test/in_n512.jpg");
	Buffer<uint8_t> tgt = Tools::load_image("../images/noise_test/in512.jpeg");
	cout<<in_b.channels()<<endl;
	cout<<tgt.channels()<<endl;

	Func in = BoundaryConditions::repeat_edge(in_b);
	Expr sig_s_f = 1.0f;
	Expr sig_r_f = 1.0f;
	
        Expr L = sig_r_f * 3.0f;
	Expr W = sig_s_f * 3.0f;

	Var x, y, t;

	// declare range and spatial filters
	Func g_sig_s, g_sig_r;
	RDom omega(-W, W, -W, W), l_r(-L, L, -L, L);;


	g_sig_s(x, y) = f32(0);
	

	g_sig_s(omega.x, omega.y) = f32(exp(-(omega.x * omega.x + omega.y * omega.y) / (2 * sig_s_f * sig_s_f)));

	g_sig_r(t) = f32(exp(- t * t / (2 * sig_r_f * sig_r_f)));


	Func imp_bi_filter, imp_bi_filter_num, imp_bi_filter_den;


	Func box_filtered;

	box_filtered(x, y) = u8((float)(1) / ((2 * L + 1) * (2 * L + 1)) * sum(in(x - l_r.x, y - l_r.y)));
        
	imp_bi_filter_num(x, y) = f32(sum(g_sig_s(omega.x, omega.y) * g_sig_r(box_filtered(x - omega.x, y - omega.y) - box_filtered(x, y)) * in(x - omega.x, y - omega.y)));
	imp_bi_filter_den(x, y) = f32(sum((g_sig_s(omega.x, omega.y)) * g_sig_r((box_filtered(x - omega.x, y - omega.y) - box_filtered(x, y)))));
	imp_bi_filter(x, y) = u8(imp_bi_filter_num(x, y) / imp_bi_filter_den(x, y));
	//imp_bi_filter.trace_stores()

	RDom r(tgt);
	Func loss;
	loss() = 0.f;
	Expr diff = in(r.x, r.y) - tgt(r.x, r.y);
	loss() += diff * diff;
	auto d_loss_d = propagate_adjoints(loss);
	
	Buffer<uint8_t> output = imp_bi_filter.realize(in_b.width(), in_b.height());

	Tools::save_image(output, "../images/noise_test/out510.jpg");

	time = clock() - time;

	printf("runtime = %f\n", (float) time / CLOCKS_PER_SEC);

}
