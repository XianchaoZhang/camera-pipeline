#include <stdio.h>
#include <math.h>
#include <complex>
#include "Halide.h"
#include "halide_image_io.h"
//#include "denoise.h"
#include <time.h>
#define R 0
#define G 1
#define B 2


using namespace Halide;
using namespace Halide::ConciseCasts;
using namespace std;


int main(int argc, char **argv) {

	Buffer<uint8_t> in_b = Tools::load_image("../images/noise_test/in_n220.jpg");
	printf("channels: %d\n", in_b.channels());
	Buffer<uint8_t> tgt = Tools::load_image("../images/noise_test/in220.png");
	//Func in = BoundaryConditions::repeat_edge(in_b);
	Var x, y, c;
	ImageParam in(UInt(8), 3);
	in(x, y, c) = in_b(x, y, c);

	Func loss;
	loss() = 0.f;
	RDom r(in_b);
	Expr diff;
	diff = in_b(r.x, r.y, G);
	loss() += diff * diff;
	auto d_loss_d = propagate_adjoints(loss);

	Func d_loss_d_in = d_loss_d(in);

	d_loss_d_in.realize(in_b.width(), in_b.height(), in_b.channels());

	// Expr sig_s;
	// Expr sig_r;
	// sig_s = 1.0f; //tune this
	// sig_r =  1.0f; //tune this
	
        // Expr L = sig_r * 3.0f;
	// Expr W = sig_s * 3.0f;

	// Var x, y, c, t;

	// // declare range and spatial filters
	// Func g_sig_s;
	// RDom omega(-W, W, -W, W), l_r(-L, L, -L, L);


	// g_sig_s(x, y) = f32(0);
        
	// g_sig_s(omega.x, omega.y) = f32(exp(-(omega.x * omega.x + omega.y * omega.y) / (2 * sig_s * sig_s)));

	// Func g_sig_r;
	// g_sig_r(t) = f32(exp(- t * t / (2 * sig_r * sig_r)));

	// Func box_filtered;
	// box_filtered(x, y, c) = u8((float)(1) / ((2 * L + 1) * (2 * L + 1)) * sum(in(x - l_r.x, y - l_r.y, c)));
	// Func imp_bi_filter_num;
	// imp_bi_filter_num(x, y, c) = f32(sum(g_sig_s(omega.x, omega.y) * g_sig_r(box_filtered(x - omega.x, y - omega.y, c) - box_filtered(x, y, c)) * in(x - omega.x, y - omega.y, c)));

	// Func imp_bi_filter_den;
	// imp_bi_filter_den(x, y, c) = f32(sum((g_sig_s(omega.x, omega.y)) * g_sig_r((box_filtered(x - omega.x, y - omega.y, c) - box_filtered(x, y, c)))));

	// Func imp_bi_filter;
	// imp_bi_filter(x, y, c) = u8(imp_bi_filter_num(x, y, c) / imp_bi_filter_den(x, y, c));
	// //imp_bi_filter.trace_stores()

	
	
	// Buffer<uint8_t> output = imp_bi_filter.realize(in_b.width(), in_b.height(), in_b.channels());

	// Tools::save_image(output, "../images/noise_test/out300.jpg");

	// time = clock() - time;

	// printf("runtime = %f\n", (float) time / CLOCKS_PER_SEC);

	// RDom r(tgt);
	// Func loss_g;
	// loss_g() = 0.f;
	// Expr diff_g = in(r.x, r.y, G) - tgt(r.x, r.y, G);
	// loss_g() += diff_g * diff_g;
	// auto d_loss_g_d = propagate_adjoints(loss_g);

	// Func d_loss_g_d_box = d_loss_g_d(box_filtered);
	// Func d_loss_g_d_f = d_loss_g_d(imp_bi_filter);

}
