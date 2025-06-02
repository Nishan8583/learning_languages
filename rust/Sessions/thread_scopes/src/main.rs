fn main() {

    const N_THREADS: usize = 8;

    let data: Vec<u32> = (0..5000).collect();


    let chunks = data.chunks(N_THREADS);

    let sum = std::thread::scope(|s| {

        let mut handles = Vec::new();

        for chunk in chunks {

            // since we are using scope, we no longer need to copy the data since using scope means rust can guarantee that threads will not outlive the data
            let handle = s.spawn(move ||{
                chunk.iter().sum::<u32>()
            });

            handles.push(handle);
        }

        handles
        .into_iter()
        .map(|h| h.join().unwrap())
        .sum::<u32>()
    });

    println!("Hello, world! SUM={}", sum);
}
