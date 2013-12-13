/* ACID WARP (c)Copyright 1992, 1993 by Noah Spurrier
 * All Rights reserved. Private Proprietary Source Code by Noah Spurrier
 * Ported to Linux by Steven Wills
 * Ported to SDL by Boris Gjenero
 */

#ifdef ENABLE_FLOAT
void generate_image_float(int imageFuncNum, UCHAR *buf_graf,
                          int _xcenter, int _ycenter,
                          int _xmax, int _ymax,
                          int colormax, int xsize, int normalize)
#else
void generate_image(int imageFuncNum, UCHAR *buf_graf,
                    int xcenter, int ycenter,
                    int xmax, int ymax,
                    int colormax, int xsize)
#endif
{

  /* WARNING!!! Major change from long to int.*/
  /* ### Changed back to long. Gives lots of warnings. Will fix soon. */

#ifndef ENABLE_FLOAT
  long /* int */ x, y, dx, dy;
  long dist, angle;
  long color;
#define _xcenter xcenter
#define _ycenter ycenter
#define _xmax xmax
#define _ymax ymax
#define _x x
#define _y y
#else /* ENABLE_FLOAT */
  long _x, _y;
  double x, y;
  double dx, dy;
  double dist, angle;
  double color;
  double xcenter, ycenter, xmax, ymax;

  if (normalize) {
    xcenter = (double)(_xcenter * 319) / _xmax;
    ycenter = (double)(_ycenter * 199) / _ymax;
    xmax = (double)319;
    ymax = (double)199;
  } else {
    xcenter = _xcenter;
    ycenter = _ycenter;
    xmax = _xmax;
    ymax = _ymax;
  }
#endif /* ENABLE_FLOAT */

  /* Some general purpose random angles and offsets.
   * Not all functions use them.
   */
  long x1,x2,x3,x4,y1,y2,y3,y4;

  x1 = RANDOM(40)-20;  x2 = RANDOM(40)-20;
  x3 = RANDOM(40)-20;  x4 = RANDOM(40)-20;
  y1 = RANDOM(40)-20;  y2 = RANDOM(40)-20;
  y3 = RANDOM(40)-20;  y4 = RANDOM(40)-20;

  for (_y = 0; _y <= _ymax; ++_y)
    {
#ifdef ENABLE_FLOAT
      if (normalize) {
        y = (double)(_y * 199) / _ymax;
      } else {
        y = _y;
      }
#endif

      for (_x = 0; _x <= _xmax; ++_x)
        {
#ifdef ENABLE_FLOAT
          if (normalize) {
            x = (double)(_x * 319) / _xmax;
          } else {
            x = _x;
          }
#endif
          dx = x - xcenter;
          /* dy may be altered below, so calculate here */
          dy = y - ycenter;

          dist  = lut_dist (dx, dy);
          angle = lut_angle (dx, dy);

          /* select function. Could be made a separate function, but since
             this function is evaluated over a large iteration of values I am
             afraid that it might slow things down even more to have a
             separate function.        */
          switch (imageFuncNum)
            {
              /* case -1:        Eight Arm Star -- produces weird discontinuity
                color = dist+ lut_sin(angle * (200 - dist)) / 32;
                                                break;
                                                */
            case 0: /* Rays plus 2D Waves */
              color = angle + lut_sin (dist * 10) / 64 +
                lut_cos (x * ANGLE_UNIT / xmax * 2) / 32 +
                lut_cos (y * ANGLE_UNIT / ymax * 2) / 32;
              break;

            case 1:        /* Rays plus 2D Waves */
              color = angle + lut_sin (dist * 10) / 16 +
                lut_cos (x * ANGLE_UNIT / xmax * 2) / 8 +
                lut_cos (y * ANGLE_UNIT / ymax * 2) / 8;
              break;

            case 2:
              color = lut_sin (lut_dist(dx + x1, dy + y1) *  4) / 32 +
                lut_sin (lut_dist(dx + x2, dy + y2) *  8) / 32 +
                lut_sin (lut_dist(dx + x3, dy + y3) * 16) / 32 +
                lut_sin (lut_dist(dx + x4, dy + y4) * 32) / 32;
              break;

            case 3:        /* Peacock */
              color = angle + lut_sin (lut_dist(dx + 20, dy) * 10) / 32 +
                angle + lut_sin (lut_dist(dx - 20, dy) * 10) / 32;
              break;

            case 4:
              color = lut_sin (dist) / 16;
              break;

            case 5:        /* 2D Wave + Spiral */
              color = lut_cos (x * ANGLE_UNIT / xmax) / 8 +
                lut_cos (y * ANGLE_UNIT / ymax) / 8 +
                angle + lut_sin(dist) / 32;
              break;

            case 6:        /* Peacock, three centers */
              color = lut_sin (lut_dist(dx,      dy - 20) * 4) / 32+
                lut_sin (lut_dist(dx + 20, dy + 20) * 4) / 32+
                lut_sin (lut_dist(dx - 20, dy + 20) * 4) / 32;
              break;

            case 7:        /* Peacock, three centers */
              color = angle +
                lut_sin (lut_dist(dx,      dy - 20) * 8) / 32+
                lut_sin (lut_dist(dx + 20, dy + 20) * 8) / 32+
                lut_sin (lut_dist(dx - 20, dy + 20) * 8) / 32;
              break;

            case 8:        /* Peacock, three centers */
              color = lut_sin (lut_dist(dx,      dy - 20) * 12) / 32+
                lut_sin (lut_dist(dx + 20, dy + 20) * 12) / 32+
                lut_sin (lut_dist(dx - 20, dy + 20) * 12) / 32;
              break;

            case 9:        /* Five Arm Star */
              color = dist + lut_sin (5 * angle) / 64;
              break;

            case 10:        /* 2D Wave */
              color = lut_cos (x * ANGLE_UNIT / xmax * 2) / 4 +
                lut_cos (y * ANGLE_UNIT / ymax * 2) / 4;
              break;

            case 11:        /* 2D Wave */
              color = lut_cos (x * ANGLE_UNIT / xmax) / 8 +
                lut_cos (y * ANGLE_UNIT / ymax) / 8;
              break;

            case 12:        /* Simple Concentric Rings */
              color = dist;
              break;

            case 13:        /* Simple Rays */
              color = angle;
              break;

            case 14:        /* Toothed Spiral Sharp */
              color = angle + lut_sin(dist * 8)/32;
              break;

            case 15:        /* Rings with sine */
              color = lut_sin(dist * 4)/32;
              break;

            case 16:        /* Rings with sine with sliding inner Rings */
              color = dist+ lut_sin(dist * 4) / 32;
              break;

            case 17:
              color = lut_sin(lut_cos(2 * x * ANGLE_UNIT / xmax)) / (20 + dist)
                + lut_sin(lut_cos(2 * y * ANGLE_UNIT / ymax)) / (20 + dist);
              break;

            case 18:        /* 2D Wave */
              color = lut_cos(7 * x * ANGLE_UNIT / xmax)/(20 + dist) +
                lut_cos(7 * y * ANGLE_UNIT / ymax)/(20 + dist);
              break;

            case 19:        /* 2D Wave */
              color = lut_cos(17 * x * ANGLE_UNIT/xmax)/(20 + dist) +
                lut_cos(17 * y * ANGLE_UNIT/ymax)/(20 + dist);
              break;

            case 20:        /* 2D Wave Interference */
              color = lut_cos(17 * x * ANGLE_UNIT / xmax) / 32 +
                lut_cos(17 * y * ANGLE_UNIT / ymax) / 32 + dist + angle;
              break;

            case 21:        /* 2D Wave Interference */
              color = lut_cos(7 * x * ANGLE_UNIT / xmax) / 32 +
                lut_cos(7 * y * ANGLE_UNIT / ymax) / 32 + dist;
              break;

            case 22:        /* 2D Wave Interference */
              color = lut_cos( 7 * x * ANGLE_UNIT / xmax) / 32 +
                lut_cos( 7 * y * ANGLE_UNIT / ymax) / 32 +
                lut_cos(11 * x * ANGLE_UNIT / xmax) / 32 +
                lut_cos(11 * y * ANGLE_UNIT / ymax) / 32;
              break;

            case 23:
              color = lut_sin (angle * 7) / 32;
              break;

            case 24:
              color = lut_sin (lut_dist(dx + x1, dy + y1) * 2) / 12 +
                lut_sin (lut_dist(dx + x2, dy + y2) * 4) / 12 +
                lut_sin (lut_dist(dx + x3, dy + y3) * 6) / 12 +
                lut_sin (lut_dist(dx + x4, dy + y4) * 8) / 12;
              break;

            case 25:
              color = angle + lut_sin (lut_dist(dx + x1, dy + y1) * 2) / 16 +
                angle + lut_sin (lut_dist(dx + x2, dy + y2) * 4) / 16 +
                lut_sin (lut_dist(dx + x3, dy + y3) * 6) /  8 +
                lut_sin (lut_dist(dx + x4, dy + y4) * 8) /  8;
              break;

            case 26:
              color = angle + lut_sin (lut_dist(dx + x1, dy + y1) * 2) / 12 +
                angle + lut_sin (lut_dist(dx + x2, dy + y2) * 4) / 12 +
                angle + lut_sin (lut_dist(dx + x3, dy + y3) * 6) / 12 +
                angle + lut_sin (lut_dist(dx + x4, dy + y4) * 8) / 12;
              break;

            case 27:
              color = lut_sin (lut_dist(dx + x1, dy + y1) * 2) / 32 +
                lut_sin (lut_dist(dx + x2, dy + y2) * 4) / 32 +
                lut_sin (lut_dist(dx + x3, dy + y3) * 6) / 32 +
                lut_sin (lut_dist(dx + x4, dy + y4) * 8) / 32;
              break;

            case 28:        /* Random Curtain of Rain (in strong wind) */
              if (y == 0 || x == 0)
                color = RANDOM (16);
              else
                color = (  *(buf_graf + (xsize *  _y   ) + (_x-1))
                         + *(buf_graf + (xsize * (_y-1)) +    _x)) / 2
                  + RANDOM (16) - 8;
              break;

            case 29:
              if (y == 0 || x == 0)
                color = RANDOM (1024);
              else
                color = dist/6 + (*(buf_graf + (xsize * _y    ) + (_x-1))
                               +  *(buf_graf + (xsize * (_y-1)) +    _x)) / 2
                + RANDOM (16) - 8;
              break;

            case 30:
              color = xor(xor(lut_sin (lut_dist(dx,     dy - 20) * 4) / 32,
                lut_sin (lut_dist(dx + 20,dy + 20) * 4) / 32),
                lut_sin (lut_dist(dx - 20,dy + 20) * 4) / 32);
              break;

            case 31:
              color = xor(mod(angle, (ANGLE_UNIT/4)), dist);
              break;

            case 32:
              color = xor(dy, dx);
              break;

            case 33:        /* Variation on Rain */
              if (y == 0 || x == 0)
                color = RANDOM (16);
              else
                color = (  *(buf_graf + (xsize *  _y   ) + (_x-1))
                         + *(buf_graf + (xsize * (_y-1)) +  _x   )  ) / 2;

              color += RANDOM (2) - 1;

              if (color < 64)
                color += RANDOM (16) - 8;
              break;

            case 34:        /* Variation on Rain */
              if (y == 0 || x == 0)
                color = RANDOM (16);
              else
                color = (  *(buf_graf + (xsize *  _y   ) + (_x-1))
                         + *(buf_graf + (xsize * (_y-1)) +  _x   )  ) / 2;

              if (color < 100)
                color += RANDOM (16) - 8;
              break;

            case 35:
              color = angle + lut_sin(dist * 8)/32;
              dx = x - xcenter;
              dy = (y - ycenter)*2;
              dist  = lut_dist (dx, dy);
          angle = lut_angle (dx, dy);
          color = (color + angle + lut_sin(dist * 8)/32) / 2;
          break;

            case 36:
              color = angle + lut_sin (dist * 10) / 16 +
                lut_cos (x * ANGLE_UNIT / xmax * 2) / 8 +
                lut_cos (y * ANGLE_UNIT / ymax * 2) / 8;
              dx = x - xcenter;
              dy = (y - ycenter)*2;
              dist  = lut_dist (dx, dy);
              angle = lut_angle (dx, dy);
              color = (color + angle + lut_sin(dist * 8)/32) / 2;
              break;

            case 37:
              color = angle + lut_sin (dist * 10) / 16 +
                lut_cos (x * ANGLE_UNIT / xmax * 2) / 8 +
                lut_cos (y * ANGLE_UNIT / ymax * 2) / 8;
              dx = x - xcenter;
              dy = (y - ycenter)*2;
              dist  = lut_dist (dx, dy);
          angle = lut_angle (dx, dy);
          color = (color + angle + lut_sin (dist * 10) / 16 +
                   lut_cos (x * ANGLE_UNIT / xmax * 2) / 8 +
                   lut_cos (y * ANGLE_UNIT / ymax * 2) / 8)  /  2;
          break;

            case 38:
#ifndef ENABLE_FLOAT
              if (dy%2)
#else
              /* Intent is to interlace two different screens */
              if (_y % 2)
#endif
                {
                  dy *= 2;
                  dist  = lut_dist (dx, dy);
                  angle = lut_angle (dx, dy);
                }
              color = angle + lut_sin(dist * 8)/32;
              break;

            case 39:
              color = xor(mod(angle, (ANGLE_UNIT/4)), dist);
              dx = x - xcenter;
              dy = (y - ycenter)*2;
              dist = lut_dist (dx, dy);
              angle = lut_angle (dx, dy);
              color = (color +  (xor(mod(angle, (ANGLE_UNIT/4)), dist))) / 2;
              break;

            case 40:
              color = xor(dy, dx);
              dx = x - xcenter;
              dy = (y - ycenter)*2;
              color = (color +  xor(dy, dx)) / 2;
              break;

            default:
              color = RANDOM (colormax - 1) + 1;
              break;
            }

          /* Fit color value into the palette range using modulo.  It seems
             that the Turbo-C MOD function does not behave the way I expect.
             It gives negative values for the MOD of a negative number.
             I expect MOD to function as it does on my HP-28S.
           */

#ifdef ENABLE_FLOAT
      /* Final colors need to go from 1 to colormax, because color 0 is
       * not used. Conversion to the palette is by truncation, not
       * rounding, as if each whole number is a bin (0 for 0 to 1,
       * 1 for 1 to 2 and so on). So, this requires:
       * 0 <= color < colormax
       */
          color = fmod(color, colormax);
#else
          color = color % (colormax-1);
#endif

          if (color < 0)
            color += (colormax - 1);

          ++color;
          /* color 0 is never used, so all colors are from 1 through 255 */

          *(buf_graf + (xsize * _y) + _x) = (UCHAR)color;
          /* Store the color in the buffer */
        }
      /* end for (y = 0; y < ymax; ++y)        */
    }
  /* end for (x = 0; x < xmax; ++x)        */

#if 0        /* For diagnosis, put palette display line at top of new image */
  for (_x = 0; _x < _xmax; ++_x)
    {
      color = (_x <= 255) ? _x : 0;

      for (_y = 0; _y < 3; ++_y)
        *(buf_graf + (xsize * _y) + _x) = (UCHAR)color;
    }
#endif
}

#ifndef ENABLE_FLOAT
#undef _xcenter
#undef _ycenter
#undef _xmax
#undef _ymax
#undef _x
#undef _y
#endif
