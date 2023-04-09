fn main() {
    println!("cargo:rerun-if-changed=src/");
    println!("cargo:rerun-if-changed=cbindgen.toml");
    cbindgen::generate(".")
        .unwrap()
        .write_to_file("../number-rust.hpp");
}
