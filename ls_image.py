#!/usr/bin/env python3


import pathlib
import subprocess
import sys


def get_terminal_size():
    """
    """
    return (188, 47, 3008, 1504)


def get_all_image_files(dirpath: pathlib.Path):
    """
    """
    output = subprocess.run(f"identify {dirpath}/* 2>/dev/null", shell=True, check=False, stdout=subprocess.PIPE)
    lines = [line.strip() for line in output.stdout.decode().split("\n")]

    for line in lines:
        if line and (not line.startswith("identify-")):
            yield pathlib.Path(line.split()[0])


def dump_images(list_path_img, w_thumb, h_thumb, p_thumb):
    """
    """
    tmp_files = []

    for idx, path_img in enumerate(list_path_img):
        command = f"convert -resize {w_thumb}x{h_thumb} -extent {w_thumb+2*p_thumb}x{h_thumb+2*p_thumb} -background black -gravity center {path_img} tmp_{idx}.png"
        tmp_files.append(f"tmp_{idx}.png")
        subprocess.run(command, shell=True, check=False)
        # convert -resize 232x168 -extent 256x192 -background black -gravity center images/9a2bb293be3ff381bc648268bdeaf509.jpg tmp_0.png

    # command = f"montage " + " ".join(tmp_files) + f" -tile {len(tmp_files)}x1 -background black -geometry {w_thumb+2*p_thumb}x{h_thumb+2*p_thumb} sixel:-"
    command = f"convert +append " + " ".join(tmp_files) + " tmp_all.png"
    subprocess.run(command, shell=True, check=False)

    command = f"img2sixel -w {len(tmp_files)*(w_thumb+2*p_thumb)}px -h {h_thumb+2*p_thumb}px tmp_all.png"
    subprocess.run(command, shell=True, check=False)


def dump_labels(list_path_img, col_thumb, row_iname):
    """
    """
    margin = 2
    lines = ["" for _ in range(row_iname)]

    for path in list_path_img:
        for idx in range(len(lines)):

            elem = path.name[idx*(col_thumb-margin):(idx+1)*(col_thumb-margin)]
            elem = elem + " " * (col_thumb - len(elem))
            lines[idx] += elem

        if len(path.name) > (row_iname * (col_thumb - margin)):
            lines[-1] = lines[-1][:-(margin+3)] + "..." + " " * margin

    for line in lines:
        if line.strip():
            print(line)
    print()


def main_dir(dirpath):
    """
    """
    col_thumb = 24
    row_thumb = 8
    row_iname = 3

    cols, rows, w_px, h_px = get_terminal_size()

    col_px, row_px = (w_px // cols, h_px // rows)

    p_thumb = int(12)   # padding
    w_thumb = int(col_thumb * col_px) - 2 * p_thumb
    h_thumb = int(row_thumb * row_px) - 2 * p_thumb

    n_image_on_a_row = w_px // (w_thumb + 2 * p_thumb) - 1
    # print("n_image_on_a_row =", n_image_on_a_row)

    list_path_img = sorted(get_all_image_files(dirpath))

    for n in range(0, len(list_path_img), n_image_on_a_row):
        dump_images(list_path_img[n:n+n_image_on_a_row], w_thumb, h_thumb, p_thumb)
        dump_labels(list_path_img[n:n+n_image_on_a_row], col_thumb, row_iname)


def main():

    path = pathlib.Path(sys.argv[1])

    if path.is_dir():
        main_dir(path)
    else:
        main_file(path)

    



main()

# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
