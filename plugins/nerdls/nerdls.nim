# Nim source code

## nerdls - `ls` like command with extra decoration using Nerd Font <https://github.com/ryanoasis/nerd-fonts>.


import std/[algorithm, math, os, osproc, parseopt, posix, sequtils, strformat, strutils, sugar]
import std/[terminal, tables, times, unicode]


# Define version string.
const VERSION = "0.2 (2021-07-21)"

# Define the default "LS_COLORS" variable as string.
const DEFAULT_LS_COLORS_NERD_S: string = """
rs=0:no=:ln=01;36:bd=40;33;01:cd=40;33;01:pi=40;33:so=01;35:ow=30;42:di=01;34:ex=01;32:
*.tar=01;31:*.tgz=01;31:*.arc=01;31:*.arj=01;31:*.taz=01;31:*.lha=01;31:*.lz4=01;31:
*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.tzo=01;31:*.t7z=01;31:*.zip=01;31:
*.z=01;31:*.dz=01;31:*.gz=01;31:*.lrz=01;31:*.lz=01;31:*.lzo=01;31:*.xz=01;31:*.zst=01;31:
*.tzst=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=01;31:*.tbz2=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:
*.jar=01;31:*.war=01;31:*.ear=01;31:*.sar=01;31:*.rar=01;31:*.alz=01;31:*.ace=01;31:*.zoo=01;31:
*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.cab=01;31:*.wim=01;31:*.swm=01;31:*.dwm=01;31:*.esd=01;31:
*.jpg=01;35:*.jpeg=01;35:*.mjpg=01;35:*.mjpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:*.pgm=01;35:
*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.ai=01;35:*.eps=01;35:*.ps=01;35:
*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:
*.webm=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;35:
*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:*.flv=01;35:*.gl=01;35:
*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:*.ogv=01;35:*.ogx=01;35:*.aac=00;36:
*.au=00;36:*.flac=00;36:*.m4a=00;36:*.mid=00;36:*.midi=00;36:*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=00;36:
*.ra=00;36:*.wav=00;36:*.wma=00;36:*.oga=00;36:*.opus=00;36:*.spx=00;36:*.xspf=00;36:
""".replace("\n", "")


# Suffix lists: data file
const ICONS_AND_SUFFIXES: seq[tuple[s: string, l: seq[string]]] = @[
    #[dat_archive]# ("\uf1c6", @[".tar", ".gz", ".tgz", ".bz", ".7z", ".zip", ".cab", ".jar", ".rar", ".lha", ".lzh"]),
    #[dat_audio  ]# ("\uf001", @[".mp3", ".wma", ".asf", ".3gp", ".3g2", ".aac", ".ogg", ".oga", ".ape", ".mac", ".wav"]),
    #[dat_excel  ]# ("\uf1c3", @[".xls", ".xlsx"]),
    #[dat_exe    ]# ("\uf013", @[".exe"]),
    #[dat_ppt    ]# ("\uf1c4", @[".ppt", ".pptx"]),
    #[dat_word   ]# ("\uf1c2", @[".doc", ".docx"]),
    #[dat_font   ]# ("\uf031", @[".ttf", ".ttc", ".otf", ".afm", ".t1"]),
    #[dat_image  ]# ("\uf1c5", @[".bmp", ".gif", ".jpg", ".jpeg", ".png", ".tif", ".tiff", ".ppm"]),
    #[dat_log    ]# ("\uf40e", @[".log"]),
    #[dat_pdf    ]# ("\uf411", @[".pdf"]),
    #[dat_vecimg ]# ("\uf857", @[".svg", ".eps", ".ps", ".dvi", ".ai"]),
    #[dat_video  ]# ("\uf03d", @[".mkv", ".flv", ".vob", ".avi", ".mov", ".wmv", ".mp4", ".mpg", ".mpeg", ".m4v", ".webm"]),
    #[src_c      ]# ("\ufb70", @[".c", ".h"]),
    #[src_css    ]# ("\uf81b", @[".css"]),
    #[src_cpp    ]# ("\ufb71", @[".cpp", ".hpp", ".cc", ".hh", ".cxx", ".hxx"]),
    #[src_csharp ]# ("\uf81a", @[".cs"]),
    #[src_go     ]# ("\ufcd1", @[".go"]),
    #[src_html   ]# ("\ue60e", @[".html", ".htm", ".xml", ".xhtml"]),
    #[src_java   ]# ("\ue738", @[".java", ".class"]),
    #[src_julia  ]# ("\ue624", @[".jl", ".julia"]),
    #[src_js     ]# ("\uf81d", @[".js"]),
    #[src_md     ]# ("\uf48a", @[".md"]),
    #[src_make   ]# ("\ue78f", @["makefile"]),
    #[src_matlab ]# ("\ue2a6", @[".m"]),
    #[src_nim    ]# ("\uf6a4", @[".nim"]),
    #[src_php    ]# ("\uf81e", @[".php"]),
    #[src_python ]# ("\uf81f", @[".py", ".pyc"]),
    #[src_r      ]# ("\ufcd2", @[".r"]),
    #[src_ruby   ]# ("\ue791", @[".rb", ".ruby"]),
    #[src_sql    ]# ("\ue7c4", @[".sql"]),
    #[src_swift  ]# ("\ufbe3", @[".swift"]),
    #[src_tex    ]# ("\uf783", @[".tex", ".sty", ".aux"]),
    #[src_ts     ]# ("\ufbe4", @[".ts"]),
    #[src_vim    ]# ("\ue7c5", @[".vim"]),
]

# Tables that map from user/group ID to user/group name.
var usrNames: Table[int, string] = {0: "root"}.toTable()
var grpNames: Table[int, string] = {0: "root"}.toTable()


type CommandlineArgs = object
    ## Object to store command line arguments.

    help     : bool
    version  : bool
    one      : bool
    all      : bool
    color    : bool
    directory: bool
    long     : bool
    human    : bool
    reverse  : bool
    recursive: bool
    groupdir : bool
    noicons  : bool
    ignopterr: bool
    maxfiles : int
    targets  : seq[string]
    colors   : seq[seq[string]]


proc parseArgs(): CommandLineArgs =
    ## Parse command line arguments.

    proc getColorMode(s: string): bool =
        ## Procedure to judge colot mode.
        if   s == "always" : return true
        elif s == "never"  : return false
        elif isatty(stdout): return true
        else               : return false

    proc optionError(args: CommandLineArgs, key: string, value: string) =
        ## Procedure when option error occurred.
        if not args.ignopterr:
            echo("nerdls: undefined option: {key}={value}".fmt)
            quit(QuitFailure)

    # Initialize command line arguments.
    result = CommandLineArgs(one: false, all: false, color: getColorMode("auto"), directory: false,
                             long: false, reverse: false, recursive: false, groupdir: false,
                             noicons: false, ignopterr: false, maxfiles: 1024, targets: @[])

    # Iniialize command line arguments.
    var parser = commandLineParams().initOptParser()

    for kind, key, value in parser.getopt():
        case kind:

            # Process ordinal arguments.
            of cmdArgument: result.targets.add(key)

            # Process option arguments.
            of cmdLongOption, cmdShortOption:
                case key:
                    of "1", "one"               : result.one       = true
                    of "a", "all"               : result.all       = true
                    of "d", "directory"         : result.directory = true
                    of "h", "human-readable"    : result.human     = true
                    of "l", "long"              : result.long      = true
                    of "r", "reverse"           : result.reverse   = true
                    of "R", "recursive"         : result.recursive = true
                    of "?", "help"              : result.help      = true
                    of "v", "version"           : result.version   = true
                    of "group-directories-first": result.groupdir  = true
                    of "noicons"                : result.noicons   = true
                    of "ignopterr"              : result.ignopterr = true
                    of "color"                  : result.color     = getColorMode(value)
                    of "maxfiles"               : result.maxfiles  = value.parseInt()
                    else                        : result.optionError(key, value)

            # Never happen.
            of cmdEnd: assert(false)

    ## If target is not specified, then set "." as a defult target.
    if result.targets.len == 0: result.targets.add(".")

    ## Parse `LS_COLOERS_NERD` which contains information for file name colorization.
    result.colors =
        if getEnv("LS_COLORS_NERD").len > 0: getEnv("LS_COLORS_NERD").split(':').map(token => token.split('='))
        else                               : DEFAULT_LS_COLORS_NERD_S.split(':').map(token => token.split('='))


proc strlen(s: string): int =
    ## Returns length of string where escape sequence will be ignored.

    # Inisialize state of escape sequence.
    var isEscSeq: bool = false

    for i, r in s.runes.toSeq:

        # Update string length.
        if r.int == 0x1b or isEscSeq: result += 0  # Escape sequence.
        elif r.int in 0x3000..0x30ff: result += 2  # Hiragana and katakana.
        elif r.int in 0x4e00..0x9fff: result += 2  # Kanji.
        elif r.int in 0xe000..0xefff: result += 1  # Half-width hiragana and katakana.
        elif r.int in 0xff00..0xffff: result += 2  # Kanji.
        else                        : result += 1

        # Update state of escape sequence.
        if   r.int == 0x1b      : isEscSeq = true
        elif r.int in 0x41..0x5a: isEscSeq = false
        elif r.int in 0x61..0x7a: isEscSeq = false


proc column(strs: seq[string], margin: int = 3): seq[string] =
    ## Returns aligned table, like the `column` command.

    proc getOptimalShape(strlens: seq[int], winWidth: int, margin: int): tuple[rows, cols: int, widths: seq[int]] =
        ## Returns optimal row/column numbers from length of strings that contains margin width.

        for delta in 0..max(0, strlens.len - 1):

            # Compute number of columns and rows.
            let numCols: int = max(1, strlens.len - delta)
            let numRows: int = max(1, strlens.len - 1) div numCols + 1

            # Compute width of each column.
            var widths = newSeqWith(numCols, 0)
            for index, length in strlens[0..<min(strlens.len, numCols * numRows)]:
                widths[index div numRows] = max(widths[index div numRows], length)

            # Returns the column numbers if it satisfies the total width constraint.
            if (widths.sum() + (numCols - 1) * margin) < winWidth:
                return (rows: numRows, cols: numCols, widths: widths)

        return (rows: strs.len, cols: 1, widths: @[winWidth])

    proc skipJoin(strs: seq[string], sep: string, skip: int, start: int = 0): string =
        ## Join string with index skipping.
        for index, s in strs:
            if index mod skip == start:
                if result.len > 0: result &= sep & s
                else             : result &= s

    # Compute length of each element.
    let strlens: seq[int] = strs.map(s => s.strlen + margin)

    # Compute optimal shape.
    let (numRows, _, widths) = getOptimalShape(strlens, terminal.terminalWidth(), margin)

    # Apply padding for each element if necessary.
    let alignedStrs: seq[string] = collect(newSeq):
        for index, s in strs: s & repeat(' ', widths[index div numRows] - s.strlen)

    # Create each lines.
    for row in 0..<numRows:
        result.add(alignedStrs.skipJoin(repeat(' ', margin), numRows, row))


proc humanReadableSize(sizeInt: BiggestInt): string =
    ## Returns human-readable format of the given integer.

    # Determine a unit of file size string.
    var size: float64 = sizeInt.float64
    var unit: char = ' '
    while unit != 'T' and size > 1024:
        case unit:
            of ' ': (size, unit) = (size / 1024, 'K')
            of 'K': (size, unit) = (size / 1024, 'M')
            of 'M': (size, unit) = (size / 1024, 'G')
            of 'G': (size, unit) = (size / 1024, 'T')
            else  : (size, unit) = (size / 1024, ' ')

    # Create file size string.
    if   unit == ' ': return $(size.int)
    elif size < 10.0: return "{size:3.1f}{unit}".fmt
    else            : return "{size.int:d}{unit}".fmt


proc getColorCode(filename: string, stat: Stat, colors: seq[seq[string]]): tuple[lhs, rhs: string] =
    ## Get Ansi color codes that is used for colorization of file name.
    ## Returned values are tuple of strings, `lhs` (left hand side) is a colorization code,
    ## and `rhs` (right hand side) is a decolorization code.

    var color_normal: string = ""
    var color_reset : string = "0"

    for pair in colors:

        # Regular code.
        if pair[0] in ["fi", "no"]:
            color_normal = pair[1]

        # Reset code.
        elif pair[0] in ["cl", "rs"]:
            color_reset = pair[1]

        # File extension code.
        elif pair[0].startswith("*."):
            if filename.endswith(pair[0][1..^1]):
                return (pair[1], color_reset)

        # File type code.
        else:
            let matched: bool = case pair[0]:
                of "bd": S_ISBLK(stat.st_mode)
                of "cd": S_ISCHR(stat.st_mode)
                of "di": S_ISDIR(stat.st_mode)
                of "ex": (stat.st_mode.uint and (S_IXUSR.uint or S_IXGRP.uint or S_IXOTH.uint)) > 0
                of "ln": S_ISLNK(stat.st_mode)
                of "no": S_ISREG(stat.st_mode)
                of "ow": (stat.st_mode.uint and S_IWOTH.uint) > 0
                of "pi": S_ISFIFO(stat.st_mode)
                of "so": S_ISSOCK(stat.st_mode)
                else   : false

            if matched: return (pair[1], color_reset)

    return (color_normal, color_reset)


proc getFileIcon(filename: string, stat: Stat, info: FileInfo): string =
    ## Returns appropriate file icon.
    ## See the cheet sheet of Nerd font for finding a hex code of icon.
    ## <https://www.nerdfonts.com/cheat-sheet>

    # Get file suffix in lower cases.
    var (_, name, suf) = splitFile(filename.toLowerAscii())

    # If suffix is empty then replace it as a file name.
    if suf.len == 0: suf = name

    # Define function which returns true if the given mode is executable.
    let S_ISEXEC = proc(mode: Mode): bool = (mode.uint and S_IXUSR.uint) > 0

    # Define function which returns true if the given mode is a symboloc link to a directory.
    let S_ISDLNK = proc(mode: Mode): bool = (S_ISLNK(mode) and (info.kind == pcDir))

    # Determine icon by file type.
    if info.kind == pcLinkToDir : return "\uf482"
    elif S_ISDLNK(stat.st_mode) : return "\uf482"
    elif S_ISBLK(stat.st_mode)  : return "\uf0c7"
    elif S_ISDIR(stat.st_mode)  : return "\uf413"
    elif S_ISFIFO(stat.st_mode) : return "\ufce3"
    elif S_ISLNK(stat.st_mode)  : return "\uf08e"
    elif S_ISCHR(stat.st_mode)  : return "\uf11c"
    elif S_ISSOCK(stat.st_mode) : return "\ufce6"

    # Determine icon by suffix.
    for (icon, suffixes) in ICONS_AND_SUFFIXES:
        if suf in suffixes: return icon

    # Determine icon by executability.
    if S_ISEXEC(stat.st_mode): return "\uf013"
    else                     : return "\uf15b"


proc getColoredNameWithIcon(path: string, stat: Stat, info: FileInfo, args: CommandLineArgs): string =
    ## Get colord file name with icon. We assumed that the font is Nerd font.

    # Get color code and file icon.
    let name = lastPathPart(path)

    # Decolate file name with file icon.
    if not args.noicons: result = getFileIcon(name, stat, info) & " " & name
    else               : result = name

    # Decolate file name with color.
    if args.color:
        let code = getColorCode(name, stat, args.colors)
        if code.lhs.len > 0: result = "\x1b[{code.lhs}m{result}\x1b[{code.rhs}m".fmt


proc decolateL(path: string, args: CommandLineArgs): seq[string] =
    ## Returns long version of decolated file name (like `ls -l`).

    # Get file infomation.
    let info: FileInfo = getFileInfo(path, followSymlink = false)

    # Get file status.
    var stat: Stat
    discard lstat(path, stat)

    # Get colored file name.
    let coloredNameWithIcon = getColoredNameWithIcon(path, stat, info, args)

    # Initialize with empty permission.
    var perm = "----------"

    # Update the first character.
    if   S_ISDIR(stat.st_mode): perm[0] = 'd'
    elif S_ISLNK(stat.st_mode): perm[0] = 'l'

    # Update permission part of the string.
    if (stat.st_mode.uint and S_IRUSR.uint) > 0: perm[1] = 'r'
    if (stat.st_mode.uint and S_IWUSR.uint) > 0: perm[2] = 'w'
    if (stat.st_mode.uint and S_IXUSR.uint) > 0: perm[3] = 'x'
    if (stat.st_mode.uint and S_IRGRP.uint) > 0: perm[4] = 'r'
    if (stat.st_mode.uint and S_IWGRP.uint) > 0: perm[5] = 'w'
    if (stat.st_mode.uint and S_IXGRP.uint) > 0: perm[6] = 'x'
    if (stat.st_mode.uint and S_IROTH.uint) > 0: perm[7] = 'r'
    if (stat.st_mode.uint and S_IWOTH.uint) > 0: perm[8] = 'w'
    if (stat.st_mode.uint and S_IXOTH.uint) > 0: perm[9] = 'x'

    # Get user name.
    if not usrNames.hasKey(stat.st_uid.int):
        usrNames[stat.st_uid.int] = execProcess("id -un {stat.st_uid.int}".fmt).strip(chars = Whitespace)

    # Get group name.
    if not grpNames.hasKey(stat.st_gid.int):
        grpNames[stat.st_gid.int] = execProcess("getent group {stat.st_gid.int} | cut -d : -f 1".fmt).strip(chars = Whitespace)

    # Create file size string.
    let sizeStr: string =
        if args.human: humanReadableSize(stat.st_size)
        else         : $(stat.st_size)

    # Create last modified date/time string.
    let mmontStr = info.lastWriteTime.format("MMM")
    let mdateStr = info.lastWriteTime.format("d")
    let mtimeStr = info.lastWriteTime.format("HH:mm")

    return @[perm, $stat.st_nlink, usrNames[stat.st_uid.int], grpNames[stat.st_gid.int], sizeStr, mmontStr, mdateStr, mtimeStr, coloredNameWithIcon]


proc decolateS(path: string, args: CommandLineArgs): string =
    ## Returns short version of decolated file name (colorization and file icon only).

    # Get file infomation.
    let info: FileInfo = getFileInfo(path, followSymlink = false)

    # Get file status.
    var stat: Stat
    discard lstat(path, stat)

    return getColoredNameWithIcon(path, stat, info, args)


proc nerdlsFile(path: string, args: CommandLineArgs) =
    ## Process one file.
    ## It is already checked that the given `path` is existing file.

    if args.long: echo(path.decolateL(args).join(" "))
    else        : echo(path.decolateS(args))


proc nerdlsDir(path: string, args: CommandLineArgs) =
    ## Process one directory.
    ## It is already checked that the given `path` is existing directory.

    # If the option `--directory` is specified, then call the function for file listing and exit this function.
    if args.directory:
        path.nerdlsFile(args)
        return

    # Flags if maximum numger of detected file exceeds threthold.
    var exceedsMax: bool = false

    # Initialize lists of detected files.
    var childrenReg: seq[tuple[kind: PathComponent, path: string]] = @[]
    var childrenDir: seq[tuple[kind: PathComponent, path: string]] = @[]

    for child in path.walkDir():

        # Skip if hidden file and `--all` is not specified.
        let name = lastPathPart(child.path)
        if (not args.all) and name.startswith('.'): continue

        # Add file to the lists.
        if child.path.dirExists(): childrenDir.add(child)
        else                     : childrenReg.add(child)

        # Abort file listing of the number of detected file exceeds the threthold.
        if (childrenReg.len + childrenDir.len) >= args.maxFiles:
            exceedsMax = true
            break

    # Do nothing if the file list is empty.
    if (childrenReg.len + childrenDir.len) == 0: return

    # Define sorting procedure.
    proc sortChildren(children: seq[tuple[kind: PathComponent, path: string]]): seq[tuple[kind: PathComponent, path: string]] =
        return children.sorted((c1, c2) => cmp(c1.path, c2.path), if args.reverse: Descending else: Ascending)

    # Sort file names for both regular files and directories.
    let sortedChildren =
        if args.groupdir: concat(childrenDir.sortChildren(), childrenReg.sortChildren())
        else            : concat(childrenDir, childrenReg).sortChildren()

    # Long expression.
    if args.long:

        # Get total size.
        let totalSize: BiggestInt = sortedChildren.map(child => child.path.getFileInfo(followSymlink = false).size).sum()
        if args.human: echo("total " & humanReadableSize(totalSize))
        else         : echo("total " & $totalSize)

        var decolatedSeqs = sortedChildren.map(child => child.path.decolateL(args))

        # Align columns by adding extra columns.
        for col in 0..max(0, decolatedSeqs[0].len - 2):
            let maxWidth = max(decolatedSeqs.map(strs => strs[col].strlen))
            for row in 0..(decolatedSeqs.len - 1):
                decolatedSeqs[row][col] = repeat(' ', maxWidth - decolatedSeqs[row][col].strlen) & decolatedSeqs[row][col]

        # Print to STDOUT.
        for strs in decolatedSeqs: echo(strs.join(" "))

    # Short expression.
    else:

        # Decolate file name and print it to STDOUT.
        let decolatedStrs = sortedChildren.map(child => child.path.decolateS(args))
        if args.one or not isatty(stdout): echo(decolatedStrs.join("\n"))
        else                             : echo(decolatedStrs.column().join("\n"))

    # Show warnings if file number thretholding applied.
    if exceedsMax:
        echo("nerdls: cannot list all files: number of files exceeds threthold")


proc nerdlsDirRecursive(path: string, args: CommandLineArgs, newline: bool = false) =
    ## Process one directory recursively.
    ## It is already checked that the given `path` is existing directory.

    echo((if newline: "\n" else: "") & path.decolateS(args) & ":")
    path.nerdlsDir(args)

    for child in path.walkDir():

        # Skip if hidden file and `--all` is not specified.
        let name = lastPathPart(child.path)
        if (not args.all) and name.startswith('.'): continue

        # Add file to the lists.
        if child.path.dirExists(): child.path.nerdlsDirRecursive(args, newline = true)
 

proc nerdls(target: string, args: CommandLineArgs) =
    ## Process one target. At this moment, it is not sure that the given target exists.

    # Expand "~" to home directory.
    let path = expandTilde(target)

    if   path.fileExists()                  : path.nerdlsFile(args)
    elif path.dirExists() and args.recursive: path.nerdlsDirRecursive(args)
    elif path.dirExists()                   : path.nerdlsDir(args)
    else                                    : echo("nerdls: cannot access '{target}': No such file or directory".fmt)


proc printHelpAndExit() =

    echo("Usage: nerdls [option...] [file...]")
    echo("")
    echo("Abailavle options:")
    echo("  -1, --one                  Print one file per line")
    echo("  -a, --all                  Do not ignore entries starting with dot")
    echo("  -d, --directory            List directories themselves, not their contents")
    echo("  --group-directories-first  Group directories before files")
    echo("  -h, --human-readable       Show file size in a human-readable format")
    echo("  --ignopterr                Ignore all undefined option error")
    echo("  -l, --long                 Print with long listing format")
    echo("  --maxfiles=int             Max number of files to be listed")
    echo("  --noicons                  Disable icons")
    echo("  --nocolor                  Disable colorization")
    echo("  -r, --reverse              Reverse order while sorting")
    echo("  -R, --recursive            List subdirectories recursively")
    echo("")
    echo("Help options:")
    echo("  -?, --help                 Show this message and exit")
    echo("  -v, --version              Show version information and exit")
    quit(QuitSuccess)


proc printVersionAndExit() =

    echo("nerdls {VERSION}".fmt)
    quit(QuitSuccess)


proc main() =
    ## Main function of `nerdls` command.

    # Initialize list of target file path and command line options.
    let args: CommandLineArgs = parseArgs()

    # Process help options.
    if   args.help   : printHelpAndExit()
    elif args.version: printVersionAndExit()

    # Process all targets.
    for target in args.targets: target.nerdls(args)


try   : main()
except: echo(getCurrentExceptionMsg())


# vim: expandtab tabstop=4 shiftwidth=4 fdm=marker syntax=nim
