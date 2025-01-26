# config.py

import datetime
import os


def get_ps0() -> str:
    """
    """
    #
    date, time = str(datetime.datetime.now()).split()

    #
    cwd = os.getcwd().replace(os.getenv("HOME"), "~")

    #
    prompt  = "\x1b[31m{user} \x1b[0m: "
    prompt += "\x1b[32m{name} \x1b[0m: "
    prompt += "\x1b[33m{date} \x1b[0m: "
    prompt += "\x1b[34m{time} \x1b[0m: "
    prompt += "\x1b[35m{cwd}"

    return prompt.format(user=os.getenv("USER"), name=os.getenv("NAME"), date=date, time=time, cwd=cwd)


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker
