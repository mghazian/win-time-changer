# Time Changer Command for Windows

This program allows the user to change machine's system time at will without going through arduous method.

## Installation

The package comes with a ready-to-use executable. However, due to different system may have different internal implementation, it is therefore recommended to build the source file yourself.

If you happen to choose to build the executable yourself, it is recommended to use `c++11` standard. `windows.h` header is required.

The executable in the package was built using gcc.

## Using The Program

The program receives any of four commands:

### `save`

Stores time into a file.

#### Command Format

```
tc.exe save <identifier>[ <Y-m-d H:i:s.ms>]
```

`<identifier>` is the name of the saved time that will be referred as.
`<Y-m-d H:i:s.ms>` is the date to be saved. Can be left out. If left out, current system time will be saved. Make sure to enclose the date with doublequotes.

#### Example

##### Save given date

```
tc.exe save birthday "1986-10-04 00:00:00.000"
```

This will create file _birthday.tc_


```
tc.exe save future "2050-12-31 22:59:00.000"
```

This will create file _future.tc_

##### Save current date

```
tc.exe save now
```

This will create file _now.tc_ that contains the time the command called

### `load`

Loads time from a file.

#### Command Format

```
tc.exe load <identifier>
```

`<identifier>` is the name of the file created from `save` command, complete with its file extension

#### Example

```
tc.exe save birthday "1986-10-04 00:00:00.000"
:: a file named 'birthday.tc' should be created
tc.exe load birthday.tc
```

### `restore`

Restores the system time before using the command, assuming no other way of changing system's time is used until the command is called. The system time will be restored as if the command is not used in the first place.

#### Command Format

```
tc.exe restore
```

#### Example

```
:: current system date 2020-10-04 12:47:10.254
:: ...make various save and load
:: ...three real life hours passed

tc.exe restore
:: system date set to 2020-10-04 15:47:10.254
```

### `clear`

Clears cache that is used for `restore`

#### Command Format

```
tc.exe clear
```

## Known Issue

- `restore` does not actually restore the time.
- `load` failed to locate the given file

The cause for both issues is mainly due to calling the command from multiple different directory. For example,

```
D:\>call C:/timechanger/bin/tc.exe save foo
:: D:\foo.tc is created
D:\>cd dir1
D:\dir1\>call C:/timechanger/bin/tc.exe load foo
:: will fail because D:\dir1\foo.tc does not exist
```

The good news is user can `load` the time just by moving to the directory containing the file (alternatively moving the file to the cwd). The bad news is user cannot `restore` the time correctly if successful `load` happened in different location.

Current suggestion is to move the cwd to where the executable is, then call the command there.

The solution for this is to stick to one place when calling the command