# General notes of the day

Duties :

- Determine the communication the arduino is using
- Understand the serial communication commands relative to the machinery
- Have the "dumped" file piped into a file to be read on the users computer
- treat as an external drive this is what we could do
- other options : computer command to dump the files over the serial line
  - instigated by the computer and then taking care of it on it's
- Look into the code David added to the slack repo

# In-use Files

- `CreateFileWriteToFile.ino`
- `RemoveFileSingleInput.ino`
  - provide name of file once at declaration and all instances are removed without needed to parse the code
  - Function was possible due to [Buffer](https://www.programmingelectronics.com/sprintf-arduino/)

# Non-Functioning Files

- `exampleLogging.ino`
