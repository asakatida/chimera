use crate::number::Number;
use criterion::{criterion_group, criterion_main, BenchmarkId, Criterion, Throughput};

pub fn criterion_benchmark(c: &mut Criterion) {
    let mut group = c.benchmark_group("pow");
    let sizes: Vec<u64> = vec![1, 1 << 1, 1 << 2, 1 << 4, 1 << 8, 1 << 16, 1 << 32, 1 << 63];
    for size in sizes.iter() {
        group.throughput(Throughput::Bytes(*size));
        group.bench_with_input(BenchmarkId::from_parameter(size), &size, |b, size| {
            b.iter(|| Number::from(size).pow(size.into()));
        });
    }
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
