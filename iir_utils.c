/*  Audio signal processing - filter (design) helpers
    © Philippe Strauss, 2012, 2013  */


#include <dspc/iir_utils.h>


/* see sp_filter_bilinear.ml for comments */

double omega_warp(double f, double fs) {
    return 2.0 * tan(f * M_PIl / fs);
}

double omega_dt2ct(double f, double fs) {
    return fs * omega_warp(f, fs);
}

/* let lp_bilin_biquad ?wc:(wc=1.0) ?kn:(kn=wc**2.) ?ctd:(c=2.0) p0 =
    let k2p0re2 = (wc ** 2.) *. (p0.re ** 2.) in
    let k2p0im2 = (wc ** 2.) *. (p0.im ** 2.) in
    let twice_ckp0 = 2. *. c *. wc *. p0.re in
    let num0 = 1.
    and num1 = 2.
    and num2 = 1. in
    let denom0 = c ** 2. +. k2p0re2 +. k2p0im2 -. twice_ckp0 
    and denom1 = 2. *. (k2p0re2 +. k2p0im2 -. c ** 2.)
    and denom2 = c ** 2. +. k2p0re2 +. k2p0im2 +. twice_ckp0 in
    (*  [IngPro] p. 403 eq. 8.48 *)
    ([|kn *. num0 /. denom0; kn *. num1 /. denom0; kn *. num2 /. denom0|],
     [|1.0; denom1 /. denom0; denom2 /. denom0|]) */

void lp_bilin_biquad(double wc, double kn, double ctd, sps_complex_d_t pole, double *b_num, double *a_denom) {
    double pre = pole.re;
    double pim = pole.im;
    double wc2 = pow(wc, 2.0);
    double ctd2 = pow(ctd, 2.0);
    double k2p0re2 = wc2 * pow(pre, 2.0);
    double k2p0im2 = wc2 * pow(pim, 2.0);
    double twice_ckp0 = 2.0 * ctd * wc * pre;
    double num0 = 1.0;
    double num1 = 2.0;
    double num2 = 1.0;
    double denom0 = ctd2 + k2p0re2 + k2p0im2 - twice_ckp0;
    double denom1 = 2.0 * (k2p0re2 + k2p0im2 - ctd2);
    double denom2 = ctd2 + k2p0re2 + k2p0im2 + twice_ckp0;
    b_num[0] = kn * num0 / denom0;
    b_num[1] = kn * num1 / denom0;
    b_num[2] = kn * num2 / denom0;
    a_denom[0] = 1.0;
    a_denom[1] = denom1 / denom0;
    a_denom[2] = denom2 / denom0;
}

/* let hp_bilin_biquad ?wc:(wc=1.0) ?kn:(kn=wc) ?ctd:(c=2.0) p0 =
    let ks = wc ** 2. in (* generic or butterworth centric ? *)
    let num0 = c ** 2. *. kn ** 2. in
    let num1 = (-2.) *. num0 in
    let num2 = num0 in
    let ks2 = ks ** 2. in
    let c2kw2p0re2 = (c ** 2.) *. (wc ** 2.) *. (p0.re ** 2.) in
    let c2kw2p0im2 = (c ** 2.) *. (wc ** 2.) *. (p0.im ** 2.) in
    let twice_cwcksp0re = 2. *. c *. wc *. ks *. p0.re in
    let denom0 = ks2 +. c2kw2p0re2 +. c2kw2p0im2 -. twice_cwcksp0re in
    let denom1 = 2. *. (ks2 -. c2kw2p0re2 -. c2kw2p0im2) in
    let denom2 = ks2 +. c2kw2p0re2 +. c2kw2p0im2 +. twice_cwcksp0re in
    ([|num0 /. denom0; num1 /. denom0; num2 /. denom0|],
     [|1.0; denom1 /. denom0; denom2 /. denom0|]) */

void hp_bilin_biquad(double wc, double kn, double ctd, sps_complex_d_t pole, double *b_num, double *a_denom) {
    double pre = pole.re;
    double pim = pole.im;
    double wc2 = pow(wc, 2.0);
    double ks = wc2;
    double ks2 = pow(ks, 2.0);
    double ctd2 = pow(ctd, 2.0);
    double c2kw2p0re2 = ctd2 * wc2 * pow(pre, 2.0);
    double c2kw2p0im2 = ctd2 * wc2 * pow(pim, 2.0);
    double twice_cwcksp0re = 2.0 * ctd * wc * ks * pre;
    double num0 = ctd2 * pow(kn, 2.0);
    double num1 = -2.0 * num0;
    double num2 = num0;
    double denom0 = ks2 + c2kw2p0re2 + c2kw2p0im2 - twice_cwcksp0re;
    double denom1 = 2.0 * (ks2 - c2kw2p0re2 - c2kw2p0im2);
    double denom2 = ks2 + c2kw2p0re2 + c2kw2p0im2 + twice_cwcksp0re;
    b_num[0] = num0 / denom0;
    b_num[1] = num1 / denom0;
    b_num[2] = num2 / denom0;
    a_denom[0] = 1.0;
    a_denom[1] = denom1 / denom0;
    a_denom[2] = denom2 / denom0;
}

/* (*  return z coeffs of 2nd order poly in descending order (or z^(-x) with x
    in increasing order), rank in tuple corresponding to x order. change of sign
    of theta due to conjugation.
    OppSch 2nd ed. p. 265 - 2012-11-8 - UNTESTED  *)

let poly2_from_conjugate r theta =
    let rcos = 2. *. r *. (cos theta) in
    let r2 = r ** 2. in
    (* z^0 , z^(-1), z^(-2) *)
    (1., -.rcos, r2) */

void poly2_from_conjugate(double r, double theta, double *poly2) {
    double rcos = 2.0 * r * cos(theta);
    double r2 = pow(r, 2.0);
    poly2[0] = 1.0;
    poly2[1] = -rcos;
    poly2[2] = r2;
}

/* (*  using mathematica : z power polynomial expand/multiplication  *)

let poly3_from_poly2 poly2a poly1 =
    let a, b, c = poly2a
    and u, v = poly1 in
    let z0 = a*.u
    and z1 = b*.u +. a*.v
    and z2 = c*.u +. b*.v
    and z3 = c*.v in
    (z0, z1, z2, z3) */

void poly3_from_poly2(const double *poly2, const double *poly1, double *poly3) {
    double a = poly2[0]; double b = poly2[1]; double c = poly2[2];
    double u = poly1[0]; double v = poly1[1];
    double z0 = a*u;
    double z1 = b*u + a*v;
    double z2 = c*u + b*v;
    double z3 = c*v;
    poly3[0] = z0;
    poly3[1] = z1;
    poly3[2] = z2;
    poly3[3] = z3;
}

/* let poly4_from_poly2 poly2a poly2b =
    let a, b, c = poly2a
    and d, e, f = poly2b in
    let z0 = a*.d
    and z1 = b*.d +. a*.e
    and z2 = c*.d +. b*.e +. a*.f
    and z3 = c*.e +. b*.f
    and z4 = c*.f in
    (z0, z1, z2, z3, z4) */

void poly4_from_poly2(const double *poly2a, const double *poly2b, double *poly4) {
    double a = poly2a[0]; double b = poly2a[1]; double c = poly2a[2];
    double d = poly2b[0]; double e = poly2b[1]; double f = poly2b[2];
    double z0 = a*d;
    double z1 = b*d + a*e;
    double z2 = c*d + b*e + a*f;
    double z3 = c*e + b*f;
    double z4 = c*f;
    poly4[0] = z0;
    poly4[1] = z1;
    poly4[2] = z2;
    poly4[3] = z3;
    poly4[4] = z4;    
}

/* let poly6_from_poly2 poly2a poly2b poly2c =
    let a, b, c = poly2a
    and d, e, f = poly2b
    and g, h, i = poly2c in
    let z0 = a*.d*.g
    and z1 = b*.d*.g +. a*.e*.g +. a*.d *.h
    and z2 = c*.d*.g +. b*.e*.g +. a*.f*.g +. b*.d*.h +. a*.e*.h +. a*.d*.i
    and z3 = c*.e*.g +. b*.f*.g +. c*.d*.h +. b*.e*.h +. a*.f*.h +. b*.d*.i +. a*.e*.i
    and z4 = c*.f*.g +. c*.e*.h +. b*.f*.h +. c*.d*.i +. b*.e*.i +. a*.f*.i
    and z5 = c*.f*.h +. c*.e*.i +. b*.f*.i
    and z6 = c*.f*.i in
    (z0, z1, z2, z3, z4, z5, z6) */

void poly6_from_poly2(const double *poly2a, const double *poly2b, const double *poly2c, double *poly6) {
    double a = poly2a[0]; double b = poly2a[1]; double c = poly2a[2];
    double d = poly2b[0]; double e = poly2b[1]; double f = poly2b[2];
    double g = poly2c[0]; double h = poly2c[1]; double i = poly2c[2];
    double z0 = a*d*g;
    double z1 = b*d*g + a*e*g + a*d*h;
    double z2 = c*d*g + b*e*g + a*f*g + b*d*h + a*e*h + a*d*i;
    double z3 = c*e*g + b*f*g + c*d*h + b*e*h + a*f*h + b*d*i + a*e*i;
    double z4 = c*f*g + c*e*h + b*f*h + c*d*i + b*e*i + a*f*i;
    double z5 = c*f*h + c*e*i + b*f*i;
    double z6 = c*f*i;
    poly6[0] = z0;
    poly6[1] = z1;
    poly6[2] = z2;
    poly6[3] = z3;
    poly6[4] = z4;
    poly6[5] = z5;
    poly6[6] = z6;
}