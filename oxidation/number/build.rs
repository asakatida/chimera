fn main() {
    println!("cargo:rerun-if-changed=src/");
    cbindgen::generate(".")
        .unwrap()
        .write_to_file("../number-rust.hpp");
}
