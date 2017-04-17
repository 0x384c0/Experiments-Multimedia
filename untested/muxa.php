<?php

$r_url = $_REQUEST['url'];
$r_count = $_REQUEST['count'];
$r_max = $_REQUEST['max'];

function is_url($str)
{
    if (!empty($str)) {
        return is_array(parse_url($str));
    }

    return false;
}

function get_random_int($min, $max)
{
    return mt_rand($min, $max);
}

function get_int_value($input, $min, $max)
{
    $i = intval($input);

    if ($i > $max) {
        $i = $max;
    } else if ($i < $min) {
        $i = $min;
    }

    return $i;
}

/**
* Resize an image and keep the proportions
* @author Allison Beckwith <allison@planetargon.com>
* @param string $filename
* @param integer $max_width
* @param integer $max_height
* @return image
*/
function resizeImage($im, $max_width, $max_height)
{
    $orig_width = imagesx($im);
    $orig_height = imagesy($im);

    $width = $orig_width;
    $height = $orig_height;

    # taller
    if ($height > $max_height) {
        $width = ($max_height / $height) * $width;
        $height = $max_height;
    }

    # wider
    if ($width > $max_width) {
        $height = ($max_width / $width) * $height;
        $width = $max_width;
    }

    $image_p = imagecreatetruecolor($width, $height);

    imagecopyresampled($image_p, $im, 0, 0, 0, 0, $width, $height, $orig_width, $orig_height);

    return $image_p;
}

if (is_url($r_url)) {
    $ch = curl_init();

    if (is_resource($ch)) {
        curl_setopt($ch, CURLOPT_URL, $r_url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_TIMEOUT, 10);

        $cnt = curl_exec($ch);

        $input = imagecreatefromstring($cnt);

        if (is_resource($input)) {
            $muha = imagecreatefrompng('muha.png');

            $w = imagesx($input);
            $h = imagesy($input);

            if(empty($r_max)) {
                $r_max = $w;

                if($w < $h) {
                    $r_max = $h;
                }
            }

            $max = get_int_value($r_max, 42, 2048);

            $input = resizeImage($input, $max, $max);

            $count = get_int_value($r_count, 1, 100);

            for ($i = 0; $i < $count; $i++) {
                $x = get_random_int(0, $w - 42);
                $y = get_random_int(0, $h - 42);

                $degrees = get_random_int(0, 360);

                $muha_rotated = imagerotate($muha, $degrees, imageColorAllocateAlpha($muha, 0, 0, 0, 127));

                imagecopy($input, $muha_rotated, $x, $y, 0, 0, 42, 42);

                imagedestroy($muha_rotated);
            }

            header('Content-Type: image/png');

            imagepng($input);
        }

        curl_close($ch);
    }
}

?>