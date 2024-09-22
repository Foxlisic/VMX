<?php

$in = file("atari.bdf", FILE_IGNORE_NEW_LINES);
$bm = [];
$ch = [];
foreach ($in as $r)
{
    if (preg_match('~ENCODING (\d+)~', $r, $c)) {
        $symcode = $c[1];
    }

    if (preg_match('~BITMAP~', $r)) {
        $bm = [];
    }

    if (preg_match('~^[0-9A-F]{2}$~i', $r, $c)) {
        $bm[] = hexdec($r);
    }

    if (preg_match('~ENDCHAR~', $r)) {
        $ch[$symcode] = $bm;
    }
}

$out = [];
$hex = [];
for ($i = 32; $i < 128; $i += 2)
{
    for ($y = 0; $y < 8; $y++)
    {
        $a = $ch[$i  ][$y];
        $b = $ch[$i+1][$y] >> 4;
        $c = $a | $b;
        $out[] = chr($c);
        $hex[] = sprintf("0x%02X", $c);
    }
}

file_put_contents("fonts.hex", join(",", $hex));
file_put_contents("fonts.bin", join("", $out));

$im = imagecreatetruecolor(384, 8);

for ($i = 32; $i < 128; $i++)
{
    for ($y = 0; $y < 8; $y++) {

        $d = $ch[$i][$y] >> 4;
        for ($j = 0; $j < 4; $j++) {

            $x = ($i-32)*4 + $j;
            imagesetpixel($im, $x, $y, $d & (16 >> $j) ? 0xFFFFFF : 0);
        }
    }
}

imagepng($im, "font.png");
