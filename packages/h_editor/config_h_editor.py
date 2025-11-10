import argparse
import sys
import re
import tomlkit
from collections import defaultdict
from pathlib import Path
from pprint import pprint

class SketchConfigHandlerException(Exception):
    def __init__(*args):
        super().__init__(*args)

class SketchConfigHandler():
    def __init__(self, header_path:Path, config_path:Path = None):
        self.header_path = header_path
        self.config_path = config_path
        self._config = tomlkit.document()
        
        if not header_path.is_file():
            raise SketchConfigHandlerException("No such header file exists!")
        
        if not config_path:
            self.config_path = self.header_path.parent / "config.toml"
        
        # Create config TOML if doesn't exist and populate with header's macros fields
        if not self.config_path.is_file():
            self.config_path.touch()            

            extracted_macros = defaultdict(dict)
            
            # Extract macros from given header
            with open(self.header_path.as_posix(), 'r') as h_f:
                cur_group = 'MISC'
                group_name_pattern = re.compile(r"/{4}(.+)")
                macro_pattern = re.compile(r"^\s*#define\s+(\w+)\s+(\w+)(?:\s*|\s+(/{2}.*))$")
                for line in h_f:
                    # print(f"line: {line}")
                    group_name_match = group_name_pattern.match(line)
                    if group_name_match:
                        # Get the group name and trim leading/trailing whitespaces
                        cur_group = group_name_match.groups()[0].strip()                   
                        print(f"cur group: {cur_group}")
                        continue
                    
                    macro_match = macro_pattern.match(line)
                    if macro_match:
                        key, value, comment = macro_match.groups()
                
                        t_key = tomlkit.key(key)
                        t_item = tomlkit.item(value)
                        # Attach comment if parsed
                        if comment:
                            t_item.comment(comment)
                            
                        extracted_macros[cur_group][t_key] = t_item
                        continue
                    
            # Add extracted contents/tables/pairs to instance TOML object
            self._config.update(extracted_macros)
                        
            # Populate and save TOML with extracted macros
            with open(self.config_path.as_posix(), 'w') as c_f:
                tomlkit.dump(self._config, c_f)
                       
        else: # TOML exists already, just load
            with open(self.config_path.as_posix(), 'r') as c_f:
                self._config = tomlkit.load(c_f)

            # pprint(dict(self.config))
       
    def retrieve(self) -> tomlkit.TOMLDocument:
        return dict(self._config)
    
    def update(self, d: dict):
        self._config.update(d)

        # Update TOML file with d
        with open(self.config_path.as_posix(), 'w') as c_f:
            tomlkit.dump(self._config, c_f)
            
    
        
            

def iter_flatten(d):
    for k, v in d.items():
        if isinstance(v, dict):
            yield from iter_flatten(v)
        else:
            yield k, v



if __name__ == "__main__":  
    parser = argparse.ArgumentParser(
        description="Update #define values in a header file from TOML config"
    )
    parser.add_argument("header", help="Path to the header file (e.g., config.h)")
    parser.add_argument("--toml", required=False, help="Path to the TOML file with updates")

    args = parser.parse_args()
    header_arg = Path(args.header)
    toml_arg = Path(args.toml) if args.toml else None

    
    handler = SketchConfigHandler(header_arg, toml_arg)

    configy = handler.retrieve()
    pprint(handler.retrieve())

    # configy['SD_CS_PIN'] += "x"
    # handler.update(configy)
    # pprint(handler.retrieve())
    
    
    # print(configy.retrieve()["adc-config"]["NUM_RESULTS"])
    