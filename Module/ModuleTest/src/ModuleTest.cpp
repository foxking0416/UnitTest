// Module test code

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ctime>
#include <utility>
#include <map>
#include "Module/Module.h"

using namespace testing;

using YM = std::pair<int, int>;
using Currency = unsigned long long;

class RLDate;

class RLDate
{
public:
    RLDate()= delete;
    RLDate( int uYear, int uMonth, int uDate )
    {
        m_uYear = uYear;
        m_uMonth = uMonth;
        m_uDate = uDate;
    }

    double DifferentSecond( const RLDate& kRHS ) const
    {
        time_t timer;
        struct tm lhs = GenerateTM( *this );
        struct tm rhs = GenerateTM( kRHS );
        return difftime( mktime( &lhs ), mktime( &rhs ) );
    }

    int operator-( const RLDate& kRHS ) const
    {
        double seconds = abs( DifferentSecond( kRHS ) );
        return seconds / 86400 + 1;
    }

    bool IsBeforeThan( const RLDate& kRHS )
    {
        return DifferentSecond( kRHS ) < 0;
    }

    bool IsEqualOrAfterThan( const RLDate& kRHS )
    {
        return DifferentSecond( kRHS ) >= 0;
    }

    int GetMonth() const
    {
        return m_uMonth;
    }

    int GetYear() const
    {
        return m_uYear;
    }

    int GetDate() const
    {
        return m_uDate;
    }

private:
    struct tm GenerateTM( const RLDate& kDate ) const
    {
        struct tm kRet ={ 0 };
        kRet.tm_year = kDate.m_uYear - 1900;
        kRet.tm_mon = kDate.m_uMonth - 1;
        kRet.tm_mday = kDate.m_uDate;
        return kRet;
    }

private:
    int m_uYear = 0;
    int m_uMonth = 0;
    int m_uDate = 0;
};

int GetMonthDayCount( const YM& YearMonth )
{
    int diff = 0;
    if ( YearMonth.second == 12 )
    {
        diff = RLDate( YearMonth.first + 1, 1, 1 ) - RLDate( YearMonth.first, YearMonth.second, 1 ) - 1;
    }
    else
    {
        diff = RLDate( YearMonth.first, YearMonth.second + 1, 1 ) - RLDate( YearMonth.first, YearMonth.second, 1 ) - 1;
    }

    return diff;
}

class RLQuota
{
public:
    RLQuota() = default;
    virtual ~RLQuota() = default;

    void SetBudget( int year, int month, const Currency nMoney )
    {
        const auto& key = std::make_pair( year, month );
        if ( m_kMap.count( key ) == 0 )
        {
            m_kMap[ key ] = nMoney;
        }
    }

    Currency Amount( RLDate kStartDate, RLDate kEndDate )
    {
        if ( kEndDate.IsBeforeThan( kStartDate ) )
        {
            RLDate kTemp = kStartDate;
            kStartDate = kEndDate;
            kEndDate = kTemp;
        }

        Currency nTotalAmount = 0;

        if ( kStartDate.GetYear() == kEndDate.GetYear() )
        {
            int nYear = kStartDate.GetYear();
            for ( int nMonth = kStartDate.GetMonth(); nMonth <= kEndDate.GetMonth(); ++nMonth )
            {
                if ( kStartDate.GetMonth() == kEndDate.GetMonth() )
                {
                    YM kYearMonth = std::make_pair( nYear, nMonth );
                    Currency kBudgetPerDay = BudgetPerDay( kYearMonth );
                    for ( int nDay = kStartDate.GetDate(); nDay <= kEndDate.GetDate(); ++nDay )
                    {
                        nTotalAmount += kBudgetPerDay;
                    }
                }
                else
                {
                    if ( nMonth == kStartDate.GetMonth() )
                    {
                        YM kYearMonth = std::make_pair( nYear, nMonth );
                        int nDayCount = GetMonthDayCount( kYearMonth );
                        Currency kBudgetPerDay = BudgetPerDay( kYearMonth );
                        for ( int nDay = kStartDate.GetDate(); nDay <= nDayCount; ++nDay )
                        {
                            nTotalAmount += kBudgetPerDay;
                        }
                    }
                    else if ( nMonth == kEndDate.GetMonth() )
                    {
                        YM kYearMonth = std::make_pair( nYear, nMonth );
                        Currency kBudgetPerDay = BudgetPerDay( kYearMonth );
                        for ( int nDay = 1; nDay <= kEndDate.GetDate(); ++nDay )
                        {
                            nTotalAmount += kBudgetPerDay;
                        }
                    }
                    else
                    {
                        YM kYearMonth = std::make_pair( nYear, nMonth );
                        nTotalAmount += m_kMap[ kYearMonth ];
                    }
                }
            }
        }
        else
        {
            for ( int nYear = kStartDate.GetYear(); nYear <= kEndDate.GetYear(); ++nYear )
            {
                if ( nYear == kStartDate.GetYear() )
                {
                    for ( int nMonth = kStartDate.GetMonth(); nMonth <= 12; ++nMonth )
                    {
                        if ( nMonth == kStartDate.GetMonth() )
                        {
                            const auto& kYearMonth = std::make_pair( nYear, nMonth );
                            int nDayCount = GetMonthDayCount( kYearMonth );
                            const auto kBudgetPerDay = BudgetPerDay( kYearMonth );
                            for ( int nDay = kStartDate.GetDate(); nDay <= nDayCount; ++nDay )
                            {
                                nTotalAmount += kBudgetPerDay;
                            }
                        }
                        else
                        {
                            const auto& kYearMonth = std::make_pair( nYear, nMonth );
                            nTotalAmount += m_kMap[ kYearMonth ];
                        }
                    }
                }
                else if ( nYear == kEndDate.GetYear() )
                {
                    for ( int nMonth = 1; nMonth <= kEndDate.GetMonth(); ++nMonth )
                    {
                        if ( nMonth == kEndDate.GetMonth() )
                        {
                            const auto& kYearMonth = std::make_pair( nYear, nMonth );
                            const auto kBudgetPerDay = BudgetPerDay( kYearMonth );
                            for ( int day = 1; day <= kEndDate.GetDate(); ++day )
                            {
                                nTotalAmount += kBudgetPerDay;
                            }
                        }
                        else
                        {
                            const auto& kYearMonth = std::make_pair( nYear, nMonth );
                            nTotalAmount += m_kMap[ kYearMonth ];
                        }
                    }
                }
                else
                {
                    for ( int nMonth = 1; nMonth <= 12; ++nMonth )
                    {
                        const auto& kYearMonth = std::make_pair( nYear, nMonth );
                        nTotalAmount += m_kMap[ kYearMonth ];
                    }
                }
            }
        }

        return nTotalAmount;
    }
private:
    Currency BudgetPerDay( const YM& YearMonth ) const
    {
        if ( m_kMap.count( YearMonth ) > 0 )
        {
            const auto kBudget = m_kMap.at( YearMonth );
            int diff = 0; 
            diff = RLDate( YearMonth.first, YearMonth.second + 1, 1 ) - RLDate( YearMonth.first, YearMonth.second, 1 ) - 1;
            assert( kBudget % diff == 0 );
            return kBudget / diff;
        }
        else
        {
            return 0;
        }
    }

private:
    std::map<YM, Currency > m_kMap;
};


TEST( CalculateDay, DayCount_1_D20190101to20190101 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 1, 1 );

    ASSERT_THAT( end - start, Eq( 1 ) );
}

TEST( CalculateDay, DayCount_2_D20190101to20190102 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 1, 2 );

    ASSERT_THAT( end - start, Eq( 2 ) );
}

TEST( CalculateDay, DayCount_2_D20190102to20190101 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 2 );
    RLDate end( 2019, 1, 1 );

    ASSERT_THAT( end - start, Eq( 2 ) );
}

TEST( CalculateDay, DayCount_32_D20190101to20190201 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 2, 1 );

    ASSERT_THAT( end - start, Eq( 32 ) );
}

TEST( CalculateDay, DayCount_29_D20190201to20190301 )
{
    // y  m  totalmoney
    RLDate start( 2019, 2, 1 );
    RLDate end( 2019, 3, 1 );

    ASSERT_THAT( end - start, Eq( 29 ) );
}

TEST( CalculateDay, DayCount_29_D21000201to21000301 )//不應該是閏年
{
    // y  m  totalmoney
    RLDate start( 2100, 2, 1 );
    RLDate end( 2100, 3, 1 );

    ASSERT_THAT( end - start, Eq( 29 ) );
}

TEST( CalculateDay, DayCount_30_D24000201to24000301 )//應該是閏年
{
    // y  m  totalmoney
    RLDate start( 2400, 2, 1 );
    RLDate end( 2400, 3, 1 );

    ASSERT_THAT( end - start, Eq( 30 ) );
}

TEST( CalculateMonthDayCount, DayCount_31_D201901 )
{
    YM kYearMonth = std::make_pair( 2019, 1 );
    ASSERT_THAT( GetMonthDayCount( kYearMonth ), Eq( 31 ) );
}

TEST( CalculateMonthDayCount, DayCount_31_D201912 )
{
    YM kYearMonth = std::make_pair( 2019, 12 );
    ASSERT_THAT( GetMonthDayCount( kYearMonth ), Eq( 31 ) );
}

TEST( GetIsOneDateBeforeAnother, D20190101_Before_D20190102 )
{
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 1, 2 );

    ASSERT_THAT( start.IsBeforeThan( end ), Eq( true ) );
}

TEST( GetIsOneDateBeforeAnother, D20190101_Before_D20190101 )
{
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 1, 1 );

    ASSERT_THAT( start.IsBeforeThan( end ), Eq( false ) );
}

TEST( GetIsOneDateEqualOrAfterAnother, D20190102_Before_D20190101 )
{
    RLDate start( 2019, 1, 2 );
    RLDate end( 2019, 1, 1 );

    ASSERT_THAT( start.IsEqualOrAfterThan( end ), Eq( true ) );
}

TEST( GetIsOneDateEqualOrAfterAnother, D20190101_Before_D20190101 )
{
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 1, 1 );

    ASSERT_THAT( start.IsEqualOrAfterThan( end ), Eq( true ) );
}

TEST( Quota, CalculateQuota_From20190101To20190102_0 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 1, 2 );

    ASSERT_THAT(end - start, Eq(2));
    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 1, 0 );
    
    ASSERT_THAT( q.Amount( start , end), Eq(0) );
}

TEST( Quota, CalculateQuota_From20190101To20190102_2 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 1, 2 );

    ASSERT_THAT( end - start, Eq( 2 ) );
    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 1, 31 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 2 ) );
}

TEST( Quota, CalculateQuota_From20190102To20190101_2 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 2 );
    RLDate end( 2019, 1, 1 );

    ASSERT_THAT( end - start, Eq( 2 ) );
    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 1, 31 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 2 ) );
}

TEST( Quota, CalculateQuota_From20190101To20191231_246 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 1 );
    RLDate end( 2019, 12, 31 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 1, 31 );
    q.SetBudget( 2019, 3, 93 );
    q.SetBudget( 2019, 5, 31 );
    q.SetBudget( 2019, 6, 60 );
    q.SetBudget( 2019, 7, 31 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 246 ) );
}

TEST( Quota, CalculateQuota_From20180101To20190630_215 )
{
    // y  m  totalmoney
    RLDate start( 2018, 1, 1 );
    RLDate end( 2019, 6, 30 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 1, 31 );
    q.SetBudget( 2019, 3, 93 );
    q.SetBudget( 2019, 5, 31 );
    q.SetBudget( 2019, 6, 60 );
    q.SetBudget( 2019, 7, 31 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 215 ) );
}

TEST( Quota, CalculateQuota_From20180101To20200315_305 )
{
    // y  m  totalmoney
    RLDate start( 2018, 1, 1 );
    RLDate end( 2020, 3, 15 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 1, 31 );
    q.SetBudget( 2019, 3, 93 );
    q.SetBudget( 2019, 5, 31 );
    q.SetBudget( 2019, 6, 60 );
    q.SetBudget( 2019, 7, 31 );
    q.SetBudget( 2020, 2, 29 );
    q.SetBudget( 2020, 3, 62 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 305 ) );
}

TEST( Quota, CalculateQuota_From20190315To20200315_232 )
{
    // y  m  totalmoney
    RLDate start( 2019, 3, 15 );
    RLDate end( 2020, 3, 15 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 1, 31 );
    q.SetBudget( 2019, 3, 93 );
    q.SetBudget( 2019, 5, 31 );
    q.SetBudget( 2019, 6, 60 );
    q.SetBudget( 2019, 7, 31 );
    q.SetBudget( 2020, 2, 29 );
    q.SetBudget( 2020, 3, 62 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 232 ) );
}


TEST( Quota, CalculateQuota_From20190301To20190331_31 )
{
    // y  m  totalmoney
    RLDate start( 2019, 3, 1 );
    RLDate end( 2019, 3, 31 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 3, 31 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 31 ) );
}

TEST( Quota, CalculateQuota_From20190201To20200228_28 )
{
    // y  m  totalmoney
    RLDate start( 2019, 2, 1 );
    RLDate end( 2019, 2, 28 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 2, 28 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 28 ) );
}

TEST( Quota, CalculateQuota_From20190215To20190315_14 )
{
    // y  m  totalmoney
    RLDate start( 2019, 2, 15 );
    RLDate end( 2019, 3, 31 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 2, 28 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 14 ) );
}

TEST( Quota, CalculateQuota_From20190301To20190301_1 )
{
    // y  m  totalmoney
    RLDate start( 2019, 3, 1 );
    RLDate end( 2019, 3, 1 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 3, 31 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 1 ) );
}

TEST( Quota, CalculateQuota_From20190228To20190301_4 )
{
    // y  m  totalmoney
    RLDate start( 2019, 2, 28 );
    RLDate end( 2019, 3, 1 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 2, 28 );
    q.SetBudget( 2019, 3, 93 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 4 ) );
}

TEST( Quota, CalculateQuota_From20190430To20190715_122 )
{
    // y  m  totalmoney
    RLDate start( 2019, 4, 30 );
    RLDate end( 2019, 7, 15 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 4, 60 );
    q.SetBudget( 2019, 6, 120 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 122 ) );
}
TEST( Quota, CalculateQuota_From20190131To20200101_2 )
{
    // y  m  totalmoney
    RLDate start( 2019, 1, 31 );
    RLDate end( 2020, 1, 1 );

    unsigned int perday = 0;
    RLQuota q;
    q.SetBudget( 2019, 1, 31 );
    q.SetBudget( 2020, 1, 1 );

    ASSERT_THAT( q.Amount( start, end ), Eq( 2 ) );
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest( &argc, argv );
    auto ret = RUN_ALL_TESTS();
    return ret;
}