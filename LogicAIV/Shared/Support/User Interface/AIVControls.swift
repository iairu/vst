import SwiftUI

struct ArcKnob: View {
    @Binding var value: Double
    var range: ClosedRange<Double>
    var title: String
    var unit: String = ""
    
    @State private var dragOffset: CGFloat = 0
    @State private var previousValue: Double = 0
    
    var normalizedValue: Double {
        (value - range.lowerBound) / (range.upperBound - range.lowerBound)
    }
    
    var body: some View {
        VStack(spacing: 8) {
            ZStack {
                // Background Track
                Circle()
                    .trim(from: 0.15, to: 0.85)
                    .stroke(Color.black.opacity(0.2), style: StrokeStyle(lineWidth: 4, lineCap: .round))
                    .rotationEffect(.degrees(90))
                    .frame(width: 60, height: 60)
                
                // Value Track
                Circle()
                    .trim(from: 0.15, to: 0.15 + (0.7 * normalizedValue))
                    .stroke(
                        LinearGradient(gradient: Gradient(colors: [Color.blue.opacity(0.8), Color.cyan]), startPoint: .topLeading, endPoint: .bottomTrailing),
                        style: StrokeStyle(lineWidth: 4, lineCap: .round)
                    )
                    .rotationEffect(.degrees(90))
                    .frame(width: 60, height: 60)
                    .shadow(color: Color.cyan.opacity(0.5), radius: 4, x: 0, y: 0)
                
                // Knob Handle logic
                Circle()
                    .fill(Color.white.opacity(0.001)) // Invisible touch target
                    .frame(width: 60, height: 60)
                    .gesture(
                        DragGesture(minimumDistance: 0)
                            .onChanged { gesture in
                                let dragChange = -gesture.translation.height / 100.0
                                let newValue = min(max(previousValue + dragChange, range.lowerBound), range.upperBound)
                                value = newValue
                            }
                            .onEnded { _ in
                                previousValue = value
                            }
                    )
                
                Text(String(format: "%.1f%@", value, unit))
                    .font(.system(size: 12, weight: .bold, design: .monospaced))
                    .foregroundColor(.white)
            }
            
            Text(title)
                .font(.caption)
                .foregroundColor(.gray)
        }
        .onAppear {
            previousValue = value
        }
    }
}

struct ModernSlider: View {
    @Binding var value: Double
    var range: ClosedRange<Double>
    var title: String
    
    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            HStack {
                Text(title)
                    .font(.caption)
                    .foregroundColor(.gray)
                Spacer()
                Text(String(format: "%.1f", value))
                    .font(.caption)
                    .foregroundColor(.white.opacity(0.8))
            }
            
            GeometryReader { geometry in
                ZStack(alignment: .leading) {
                    Rectangle()
                        .fill(Color.black.opacity(0.2))
                        .frame(height: 4)
                        .cornerRadius(2)
                    
                    Rectangle()
                        .fill(LinearGradient(gradient: Gradient(colors: [Color.blue, Color.cyan]), startPoint: .leading, endPoint: .trailing))
                        .frame(width: geometry.size.width * CGFloat((value - range.lowerBound) / (range.upperBound - range.lowerBound)), height: 4)
                        .cornerRadius(2)
                        .shadow(color: Color.cyan.opacity(0.5), radius: 2)
                }
            }
            .frame(height: 10)
    }
}

struct EffectGroup<Content: View>: View {
    var title: String
    var content: Content
    
    init(title: String, @ViewBuilder content: () -> Content) {
        self.title = title
        self.content = content()
    }
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text(title)
                .font(.system(size: 10, weight: .bold))
                .foregroundColor(.cyan)
                .tracking(1)
            
            VStack {
                content
            }
            .padding()
            .background(Color.black.opacity(0.3))
            .cornerRadius(8)
            .overlay(
                RoundedRectangle(cornerRadius: 8)
                    .stroke(Color.white.opacity(0.1), lineWidth: 1)
            )
        }
    }
}

