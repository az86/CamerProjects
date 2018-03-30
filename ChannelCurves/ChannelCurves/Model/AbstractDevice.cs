namespace ChannelCurves.Model
{
    abstract class AbstractDevice
    {
        public byte[] XferData()
        {
            return OnXferData();
        }

        protected abstract byte[] OnXferData();
    }
}
